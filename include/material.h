//
// Created by 何振邦 on 2019-05-15.
//

#ifndef RTTEST_MATERIAL_H
#define RTTEST_MATERIAL_H

#include "ray.h"
#include "sampler.h"
#include "texture.h"
#include <memory>

enum MATERIAL_TYPE {
	REFLECTION = 1 << 0,
	TRANSMISSION = 1 << 1,
	DIFFUSE = 1 << 2
};

struct Material {
	int type;
	glm::vec3 emitDirection;//vec3(0) means emitting rays everywhere
	std::shared_ptr<Texture<glm::vec3>> emissionTex;
	std::shared_ptr<Texture<glm::vec3>> albedoTex;
	std::shared_ptr<Texture<float>> alphaTex;
	static std::shared_ptr<Sampler> defaultSampler;
	std::shared_ptr<Sampler> sampler;

	Material() {
		sampler = defaultSampler;
		emissionTex = nullptr;
		albedoTex = nullptr;
		alphaTex = nullptr;
		emitDirection = glm::vec3(0);
	}

	virtual glm::vec3
	brdf(const glm::vec3 &_inRay, const glm::vec3 &_outRay, const HitInfo &_hitInfo) const { return glm::vec3(0); }

	virtual glm::vec3 albedo(const glm::vec2 &_uv) const {
		return albedoTex == nullptr ? glm::vec3(1) : albedoTex->getColor(_uv);
	}

	virtual glm::vec3 emitted(const Ray &_ray, const glm::vec2 &_uv) const {
		glm::vec3 e = emissionTex == nullptr ? glm::vec3(0) : emissionTex->getColor(_uv);
		return glm::dot(emitDirection, _ray.dir) < 0 ? e :
		       (std::fabs(emitDirection[0]) + std::fabs(emitDirection[1]) + std::fabs(emitDirection[2]) < 0.0001 ?
		        e : glm::vec3(0));
	}

	//Standby, only for test now, do not use it!
	virtual glm::vec3 emitted(const Ray &ray, const HitInfo &hitInfo) const {
		glm::vec3 e = emissionTex == nullptr ? glm::vec3(0) : emissionTex->getColor(hitInfo.uv);
		return glm::dot(hitInfo.normal, ray.dir) < 0 ? e : glm::vec3(0);
	}

	virtual bool scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const = 0;

	virtual bool
	scatterPro(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const = 0;
};

//NOTE. Lambertian does not scatter ray uniformly at hemisphere space, ray intensity obeys Lambert Cosine Law
//What is Lambert Cosine Law? See https://baike.baidu.com/item/朗伯辐射体/6224491
//https://en.wikipedia.org/wiki/Lambert%27s_cosine_law
//https://en.wikipedia.org/wiki/Lambertian_reflectance
struct Lambertian : Material {
	Lambertian() {
		type = MATERIAL_TYPE::DIFFUSE;
	}

	Lambertian(glm::vec3 _albedo, glm::vec3 _emission = glm::vec3(0));

	Lambertian(std::shared_ptr<Texture<glm::vec3>> _albedoTex,
	           std::shared_ptr<Texture<glm::vec3>> _emissionTex = nullptr);

	glm::vec3 brdf(const glm::vec3 &_inRay, const glm::vec3 &_outRay, const HitInfo &_hitInfo) const override;

	bool scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const override;

	bool scatterPro(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const override;
};

struct Metal : Material {
	std::shared_ptr<Texture<float>> fuzz;

	Metal() {
		type = MATERIAL_TYPE::REFLECTION;
		fuzz = nullptr;
	}

	Metal(glm::vec3 _albedo, float _fuzz = 0.0);

	Metal(glm::vec3 _albedo, std::shared_ptr<Texture<float>> _fuzz);

	Metal(std::shared_ptr<Texture<glm::vec3>> _albedo, std::shared_ptr<Texture<float>> _fuzz);

	bool scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const override;

	bool scatterPro(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const override;
};

struct Dielectric : Material {
	float refractIndex;
	std::shared_ptr<Texture<float>> reflectFuzz;
	std::shared_ptr<Texture<float>> refractFuzz;

	Dielectric() {
		type = MATERIAL_TYPE::REFLECTION | MATERIAL_TYPE::TRANSMISSION;
		reflectFuzz = nullptr;
		refractFuzz = nullptr;
	}

	Dielectric(glm::vec3 _albedo, float _refractIndex, float _reflectFuzz = 0.0, float _refractFuzz = 0.0);

	Dielectric(std::shared_ptr<Texture<glm::vec3>> _albedo, float _refractIndex,
	           std::shared_ptr<Texture<float>> _reflectFuzz = nullptr,
	           std::shared_ptr<Texture<float>> _refractFuzz = nullptr);

	bool scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const override;

	bool scatterPro(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const override;
};

struct Isotropy : Material {
	Isotropy() {
		type = MATERIAL_TYPE::DIFFUSE;
	}

	Isotropy(glm::vec3 _albedo);

	bool scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const override;

	bool scatterPro(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const override;
};


//NOTE. EnivronmentMap material dose not scatter ray because it is a light source;
//      EnvironmentMap uses albedoTex as intensity measurement of emissionTex since EnvironmentMap does not do scattering
//      and does not need albedoTex data.
struct EnvironmentMap : Material {
	EnvironmentMap() {
		type = MATERIAL_TYPE::DIFFUSE;
	}

	//Be careful about parameter order
	EnvironmentMap(glm::vec3 _emission, glm::vec3 _albedo = glm::vec3(1));

	EnvironmentMap(std::shared_ptr<Texture<glm::vec3>> _emissionTex,
	               std::shared_ptr<Texture<glm::vec3>> _albedoTex = nullptr);

	glm::vec3 albedo(const glm::vec2 &_uv) const override;

	glm::vec3 emitted(const Ray &_ray, const glm::vec2 &_uv) const override;

	glm::vec3 brdf(const glm::vec3 &_inRay, const glm::vec3 &_outRay, const HitInfo &_hitInfo) const override;

	bool scatter(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const override;

	bool scatterPro(const Ray &ray, const HitInfo &hitInfo, glm::vec3 *attenuation, Ray *scatteredRay) const override;
};

#endif //RTTEST_MATERIAL_H
