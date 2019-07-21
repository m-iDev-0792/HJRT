//
// Created by 何振邦 on 2019-05-15.
//

#ifndef RTTEST_MATERIAL_H
#define RTTEST_MATERIAL_H

#include "ray.h"
#include "utility.h"
#include "texture.h"
#include <memory>

struct Material {
	glm::vec3 emission;
	glm::vec3 emitDirection;//vec3(0) means emitting rays everywhere
	glm::vec3 albedo;
	std::shared_ptr<Texture> emissionTex;
	std::shared_ptr<Texture> albedoTex;

	virtual glm::vec3 albedoed(const glm::vec2 &uv) const {
		return albedoTex == nullptr ? albedo : albedoTex->getColor(uv);
	}
	virtual glm::vec3 emitted(const Ray &ray, const glm::vec2 &uv) const {
		glm::vec3 e = emissionTex == nullptr ? emission : emissionTex->getColor(uv);
		return glm::dot(emitDirection, ray.dir) < 0 ? e :
		       (std::fabs(emitDirection[0]) + std::fabs(emitDirection[1]) + std::fabs(emitDirection[2]) < 0.0001 ?
		        e : glm::vec3(0));
	}
	//Standby, only for test now, do not use it!
	virtual glm::vec3 emitted(const Ray &ray, const HitInfo &hitInfo) const {
		glm::vec3 e = emissionTex == nullptr ? emission : emissionTex->getColor(hitInfo.uv);
		return glm::dot(hitInfo.normal, ray.dir) < 0 ? e : glm::vec3(0);
	}

	virtual bool scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const = 0;
};

//NOTE. Lambertian does not scatter ray uniformly at hemisphere space, ray intensity obeys Lambert Cosine Law
//What is Lambert Cosine Law? See https://baike.baidu.com/item/朗伯辐射体/6224491
//https://en.wikipedia.org/wiki/Lambert%27s_cosine_law
//https://en.wikipedia.org/wiki/Lambertian_reflectance
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
