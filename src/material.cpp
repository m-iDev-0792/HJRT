//
// Created by 何振邦 on 2019-05-15.
//

#include "material.h"

std::shared_ptr<Sampler> Material::defaultSampler = std::make_shared<CosineHemisphereSampler>();

Lambertian::Lambertian(glm::vec3 _albedo, glm::vec3 _emission) {
	albedoTex = std::make_shared<SolidColorTexture>(_albedo);
	emissionTex = std::make_shared<SolidColorTexture>(_emission);
}

Lambertian::Lambertian(std::shared_ptr<Texture> _albedoTex, std::shared_ptr<Texture> _emissionTex) {
	albedoTex = _albedoTex;
	emissionTex = _emissionTex;
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
	if (glm::dot(scatteredRay->dir, hitInfo.normal) < 0 || samplingPDF <= 0)return false;//invalid ray sampling
	//rendering equation
	*attenuation =
			brdf(ray.dir, scatteredRay->dir, hitInfo) * glm::dot(hitInfo.normal, scatteredRay->dir) / samplingPDF;
	return true;
}

Metal::Metal(glm::vec3 _albedo, float _fuzz) {
	albedoTex = std::make_shared<SolidColorTexture>(_albedo);
	emissionTex = std::make_shared<SolidColorTexture>(glm::vec3(0));
	fuzz = (_fuzz > 1) ? 1 : _fuzz;
}

bool Metal::scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const {
	if (glm::dot(ray.dir, hitInfo.normal) >= 0)return false;
	scatteredRay->origin = hitInfo.hitpoint;
	scatteredRay->time = ray.time;
	scatteredRay->tMin = ray.tMin;
	scatteredRay->tMax = ray.tMax;
	if (fuzz == 0)scatteredRay->dir = glm::normalize(reflect(ray.dir, hitInfo.normal));
	else scatteredRay->dir = glm::normalize(reflect(ray.dir, hitInfo.normal) + fuzz * sampleInsideSphereUniform());
	*attenuation = albedo(hitInfo.uv);
	return (dot(scatteredRay->dir, hitInfo.normal) > 0);
}

bool Metal::scatterPro(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const {
	return scatter(ray, hitInfo, attenuation, scatteredRay);
}

Dielectric::Dielectric(float _refractIndex) : refractIndex(_refractIndex) {
	albedoTex = std::make_shared<SolidColorTexture>(glm::vec3(1));
	emissionTex = std::make_shared<SolidColorTexture>(glm::vec3(0));
}

bool Dielectric::scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const {
	*attenuation = albedo(hitInfo.uv);
	float ni_over_nr, cosine, reflect_prob;
	float dotNormalIncidence = glm::dot(ray.dir, hitInfo.normal);
	glm::vec3 outwardNormal;
	if (dotNormalIncidence > 0) {
		//incidence and normal are not in the same side
		outwardNormal = -hitInfo.normal;
		ni_over_nr = refractIndex;
		cosine = refractIndex * dotNormalIncidence;//TODO. figure out why multiply refractIndex??????
	} else {
		//the most common case
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
		scatteredRay->dir = glm::normalize(reflect(ray.dir, hitInfo.normal));
		scatteredRay->tMin = ray.tMin;
		scatteredRay->tMax = ray.tMax;
	} else {
		//do refract
		scatteredRay->origin = hitInfo.hitpoint;
		scatteredRay->time = ray.time;
		scatteredRay->dir = glm::normalize(refracted);
		scatteredRay->tMin = ray.tMin;
		scatteredRay->tMax = ray.tMax;
	}

	return true;
}

bool Dielectric::scatterPro(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const {
	return scatter(ray, hitInfo, attenuation, scatteredRay);
}

Isotropy::Isotropy(glm::vec3 _albedo) {
	albedoTex = std::make_shared<SolidColorTexture>(_albedo);
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