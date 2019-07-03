//
// Created by 何振邦 on 2019-05-15.
//

#ifndef RTTEST_MATERIAL_H
#define RTTEST_MATERIAL_H

#include "utility.h"
#include "texture.h"
#include <memory>

struct Material {
	glm::vec3 emission;
	glm::vec3 albedo;
	std::shared_ptr<Texture> emissionTex;
	std::shared_ptr<Texture> albedoTex;

	virtual bool scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const = 0;
};

struct Lambertian : Material {
	Lambertian() = default;

	Lambertian(glm::vec3 _albedo);

	bool scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const override;
};

struct Metal : Material {
	float fuzz;

	Metal() = default;

	Metal(glm::vec3 _albedo, float _fuzz = 0.2);

	bool scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const override;
};

struct Dielectric : Material {
	float refractIndex;

	Dielectric() = default;

	Dielectric(float _refractIndex);

	bool scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const override;
};

struct Isotropy : Material {
	Isotropy() = default;

	Isotropy(glm::vec3 _albedo);

	bool scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const override;
};

#endif //RTTEST_MATERIAL_H
