//
// Created by 何振邦 on 2019-05-15.
//

#include "material.h"

std::shared_ptr<Sampler> Material::defaultSampler = std::make_shared<CosineHemisphereSampler>();
//----------------------------------
//            Lambertian
//----------------------------------
Lambertian::Lambertian(glm::vec3 _albedo, glm::vec3 _emission) {
	albedoTex = std::make_shared<SolidColorTexture<glm::vec3>>(_albedo);
	emissionTex = std::make_shared<SolidColorTexture<glm::vec3>>(_emission);
	alphaTex = nullptr;
}

Lambertian::Lambertian(std::shared_ptr<Texture<glm::vec3>> _albedoTex,
                       std::shared_ptr<Texture<glm::vec3>> _emissionTex) {
	albedoTex = _albedoTex;
	emissionTex = _emissionTex;
	alphaTex = nullptr;
}

glm::vec3 Lambertian::brdf(const glm::vec3 &_inRay, const glm::vec3 &_outRay, const HitInfo &_hitInfo) const {
	return albedo(_hitInfo.uv) * static_cast<float>(C_INV_PI);
}

bool Lambertian::scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const {
	if (glm::dot(ray.dir, hitInfo.normal) >= 0)return false;
	scatteredRay->origin = hitInfo.hitpoint;
	scatteredRay->dir = glm::normalize(hitInfo.normal + sampleInsideSphereUniform());
	scatteredRay->time = ray.time;
	scatteredRay->tMin = ray.tMin;
	scatteredRay->tMax = ray.tMax;
	*attenuation = albedo(hitInfo.uv);
	return true;
}

bool Lambertian::scatterPro(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const {
	if (glm::dot(ray.dir, hitInfo.normal) >= 0)return false;
	scatteredRay->origin = hitInfo.hitpoint;
	scatteredRay->time = ray.time;
	scatteredRay->tMin = ray.tMin;
	scatteredRay->tMax = ray.tMax;
	float samplingPDF = sampler->sample(hitInfo, &(scatteredRay->dir));
	if (glm::dot(scatteredRay->dir, hitInfo.normal) < 0.00001 || samplingPDF <= 0)return false;//invalid ray sampling
	//rendering equation
	*attenuation =
			brdf(ray.dir, scatteredRay->dir, hitInfo) * glm::dot(hitInfo.normal, scatteredRay->dir) / samplingPDF;
	return true;
}

//----------------------------------
//              Metal
//----------------------------------
Metal::Metal(glm::vec3 _albedo, float _fuzz) {
	albedoTex = std::make_shared<SolidColorTexture<glm::vec3>>(_albedo);
	emissionTex = std::make_shared<SolidColorTexture<glm::vec3>>(glm::vec3(0));
	alphaTex = nullptr;
	float f = (_fuzz > 1) ? 1 : _fuzz;
	fuzz = std::make_shared<SolidColorTexture<float>>(f);
}

Metal::Metal(glm::vec3 _albedo, std::shared_ptr<Texture<float>> _fuzz) {
	albedoTex = std::make_shared<SolidColorTexture<glm::vec3>>(_albedo);
	emissionTex = std::make_shared<SolidColorTexture<glm::vec3>>(glm::vec3(0));
	alphaTex = nullptr;
	fuzz = _fuzz;
}

Metal::Metal(std::shared_ptr<Texture<glm::vec3>> _albedo, std::shared_ptr<Texture<float>> _fuzz) {
	albedoTex = _albedo;
	emissionTex = std::make_shared<SolidColorTexture<glm::vec3>>(glm::vec3(0));
	alphaTex = nullptr;
	fuzz = _fuzz;
}

bool Metal::scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const {
	if (glm::dot(ray.dir, hitInfo.normal) >= 0)return false;
	scatteredRay->origin = hitInfo.hitpoint;
	scatteredRay->time = ray.time;
	scatteredRay->tMin = ray.tMin;
	scatteredRay->tMax = ray.tMax;
	float FUZZ = fuzz->getColor(hitInfo.uv);

	if (FUZZ > 0.001)
		scatteredRay->dir = glm::normalize(reflect(ray.dir, hitInfo.normal) + FUZZ * sampleInsideSphereUniform());
	else scatteredRay->dir = glm::normalize(reflect(ray.dir, hitInfo.normal));

	*attenuation = albedo(hitInfo.uv);
	return (dot(scatteredRay->dir, hitInfo.normal) > 0);
}

bool Metal::scatterPro(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const {
	return scatter(ray, hitInfo, attenuation, scatteredRay);
}

//----------------------------------
//           Dielectirc
//----------------------------------
Dielectric::Dielectric(glm::vec3 _albedo, float _refractIndex, float _reflectFuzz, float _refractFuzz) : refractIndex(
		_refractIndex) {
	albedoTex = std::make_shared<SolidColorTexture<glm::vec3>>(_albedo);
	reflectFuzz = std::make_shared<SolidColorTexture<float>>(_reflectFuzz);
	refractFuzz = std::make_shared<SolidColorTexture<float>>(_refractFuzz);
	emissionTex = std::make_shared<SolidColorTexture<glm::vec3>>(glm::vec3(0));
	alphaTex = nullptr;
}

Dielectric::Dielectric(std::shared_ptr<Texture<glm::vec3>> _albedo, float _refractIndex,
                       std::shared_ptr<Texture<float>> _reflectFuzz,
                       std::shared_ptr<Texture<float>> _refractFuzz) : refractIndex(_refractIndex) {
	albedoTex = _albedo;
	reflectFuzz = _reflectFuzz == nullptr ? std::make_shared<SolidColorTexture<float>>(0) : _reflectFuzz;
	refractFuzz = _refractFuzz == nullptr ? std::make_shared<SolidColorTexture<float>>(0) : _refractFuzz;
	emissionTex = std::make_shared<SolidColorTexture<glm::vec3>>(glm::vec3(0));
	alphaTex = nullptr;
}

bool Dielectric::scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const {
	*attenuation = albedo(hitInfo.uv);
	float ni_over_nr, cosine, reflect_prob;
	float dotNormalIncidence = glm::dot(ray.dir, hitInfo.normal);
	glm::vec3 outwardNormal;
	if (dotNormalIncidence > 0) {
		//incidence and normal are not in the same side, from inside to outside
		outwardNormal = -hitInfo.normal;
		ni_over_nr = refractIndex;
		cosine = refractIndex * dotNormalIncidence;//TODO. figure out why multiply refractIndex??????
	} else {
		//the most common case, from outside to inside
		outwardNormal = hitInfo.normal;
		ni_over_nr = 1.0 / refractIndex;
		cosine = -dotNormalIncidence;
	}
	glm::vec3 refracted;
	//calculate reflect probability
	if (refract(ray.dir, outwardNormal, ni_over_nr, &refracted)) {
		reflect_prob = fresnel(cosine, refractIndex);
	} else {
		reflect_prob = 1.0;
	}

	if (random0_1f() < reflect_prob) {
		//do reflect
		scatteredRay->origin = hitInfo.hitpoint;
		scatteredRay->time = ray.time;

		float FUZZ = reflectFuzz->getColor(hitInfo.uv);
		if (FUZZ > 0.001)
			scatteredRay->dir = glm::normalize(
					glm::normalize(reflect(ray.dir, hitInfo.normal)) + FUZZ * sampleInsideSphereUniform());
		else scatteredRay->dir = glm::normalize(reflect(ray.dir, hitInfo.normal));

		scatteredRay->tMin = ray.tMin;
		scatteredRay->tMax = ray.tMax;
	} else {
		//do refract
		scatteredRay->origin = hitInfo.hitpoint;
		scatteredRay->time = ray.time;

		float FUZZ = refractFuzz->getColor(hitInfo.uv);
		if (FUZZ > 0.001)
			scatteredRay->dir = glm::normalize(glm::normalize(refracted) + FUZZ * sampleInsideSphereUniform());
		else scatteredRay->dir = glm::normalize(refracted);

		scatteredRay->tMin = ray.tMin;
		scatteredRay->tMax = ray.tMax;
	}

	return true;
}

bool Dielectric::scatterPro(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const {
	return scatter(ray, hitInfo, attenuation, scatteredRay);
}

//----------------------------------
//             Isotropy
//----------------------------------
Isotropy::Isotropy(glm::vec3 _albedo) {
	albedoTex = std::make_shared<SolidColorTexture<glm::vec3>>(_albedo);
}

bool Isotropy::scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const {
	*attenuation = albedo(hitInfo.uv);
	scatteredRay->origin = ray.origin;
	scatteredRay->time = ray.time;
	scatteredRay->dir = sampleInsideSphereUniform();
	scatteredRay->tMin = ray.tMin;
	scatteredRay->tMax = ray.tMax;
	return true;
}

bool Isotropy::scatterPro(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const {
	return scatter(ray, hitInfo, attenuation, scatteredRay);
}

//----------------------------------
//         EnvironmentMap
//----------------------------------
EnvironmentMap::EnvironmentMap(glm::vec3 _emission, glm::vec3 _albedo) {
	emissionTex = std::make_shared<SolidColorTexture<glm::vec3>>(_emission);
	albedoTex = std::make_shared<SolidColorTexture<glm::vec3>>(_albedo);
}

EnvironmentMap::EnvironmentMap(std::shared_ptr<Texture<glm::vec3>> _emissionTex,
                               std::shared_ptr<Texture<glm::vec3>> _albedoTex) {
	emissionTex = _emissionTex;
	if (_albedoTex == nullptr)
		albedoTex = std::make_shared<SolidColorTexture<glm::vec3>>(glm::vec3(1));
	else
		albedoTex = _albedoTex;
}

glm::vec3 EnvironmentMap::brdf(const glm::vec3 &_inRay, const glm::vec3 &_outRay, const HitInfo &_hitInfo) const {
	return glm::vec3(1);
}

bool EnvironmentMap::scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const {
	//environment map does not do scattering cause we already hit a light source
	return false;
}

bool EnvironmentMap::scatterPro(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation,
                                Ray *scatteredRay) const {
	//environment map does not do scattering cause we already hit a light source
	return false;
}

glm::vec3 EnvironmentMap::albedo(const glm::vec2 &_uv) const {
	return glm::vec3(1);
}

glm::vec3 EnvironmentMap::emitted(const Ray &_ray, const glm::vec2 &_uv) const {
	glm::vec3 e = emissionTex == nullptr ? glm::vec3(0) : emissionTex->getColor(_uv);
	return albedoTex->getColor(_uv) * (glm::dot(emitDirection, _ray.dir) < 0 ? e :
	                                   (std::fabs(emitDirection[0]) + std::fabs(emitDirection[1]) +
	                                    std::fabs(emitDirection[2]) < 0.0001 ?
	                                    e : glm::vec3(0)));
}