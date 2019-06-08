//
// Created by 何振邦 on 2019-05-15.
//

#include "material.h"

Lambertian::Lambertian(glm::vec3 _albedo) {
	emission = glm::vec3(0);
	albedo = _albedo;
}

bool Lambertian::scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const {
	scatteredRay->origin = hitInfo.hitpoint;
	scatteredRay->dir = glm::normalize(hitInfo.normal + randomVecUnitSphere());
	*attenuation = (albedoTex == nullptr ? albedo : albedoTex->getColor(hitInfo.uv));
	return true;
}

Metal::Metal(glm::vec3 _albedo, float _fuzz) {
	albedo = _albedo;
	emission = glm::vec3(0);
	fuzz = (_fuzz > 1) ? 1 : _fuzz;
}

bool Metal::scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const {
	scatteredRay->origin = hitInfo.hitpoint;
	if (fuzz == 0)scatteredRay->dir = glm::normalize(reflect(ray.dir, hitInfo.normal));
	else scatteredRay->dir = glm::normalize(reflect(ray.dir, hitInfo.normal) + fuzz * randomVecUnitSphere());
	*attenuation = (albedoTex == nullptr ? albedo : albedoTex->getColor(hitInfo.uv));
	return (dot(scatteredRay->dir, hitInfo.normal) > 0);
}

Dielectric::Dielectric(float _refractIndex) : refractIndex(_refractIndex) {
	emission = glm::vec3(0);
	albedo = glm::vec3(1);
}

bool Dielectric::scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const {
	*attenuation = (albedoTex == nullptr ? albedo : albedoTex->getColor(hitInfo.uv));
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
		scatteredRay->dir = glm::normalize(reflect(ray.dir, hitInfo.normal));
	} else {
		//do refract
		scatteredRay->origin = hitInfo.hitpoint;
		scatteredRay->dir = glm::normalize(refracted);
	}

	return true;
}