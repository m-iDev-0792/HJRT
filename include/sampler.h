//
// Created by 何振邦 on 2019-07-21.
//

#ifndef HJRT_SAMPLER_H
#define HJRT_SAMPLER_H

#include "utility.h"
#include "onb.h"
#include "ray.h"
#include "shape.h"

struct Sampler {
	virtual float pdf(const HitInfo &_hitInfo, const glm::vec3 &_direction) const = 0;

	virtual float sample(const HitInfo &_hitInfo, glm::vec3 *_sampledDirection) const = 0;
};

struct UniformHemisphereSampler : Sampler {
	UniformHemisphereSampler() = default;

	float pdf(const HitInfo &_hitInfo, const glm::vec3 &_direction) const override;

	float sample(const HitInfo &_hitInfo, glm::vec3 *_sampledDirection) const override;
};

struct CosineHemisphereSampler : Sampler {
	CosineHemisphereSampler() = default;

	float pdf(const HitInfo &_hitInfo, const glm::vec3 &_direction) const override;

	float sample(const HitInfo &_hitInfo, glm::vec3 *_sampledDirection) const override;
};

struct ObjectSampler : Sampler {
	std::shared_ptr<SampleableShape> target;

	ObjectSampler() = default;

	ObjectSampler(std::shared_ptr<SampleableShape> _target) : target(_target) {}

	float pdf(const HitInfo &_hitInfo, const glm::vec3 &_direction) const override;

	float sample(const HitInfo &_hitInfo, glm::vec3 *_sampledDirection) const override;
};

struct MixtureSampler : Sampler {
	std::shared_ptr<Sampler> sampler1;
	std::shared_ptr<Sampler> sampler2;
	float p1;

	MixtureSampler(std::shared_ptr<Sampler> _sampler1, std::shared_ptr<Sampler> _sampler2, float probability)
			: sampler1(_sampler1), sampler2(_sampler2), p1(probability) {}

	float pdf(const HitInfo &_hitInfo, const glm::vec3 &_direction) const override;

	float sample(const HitInfo &_hitInfo, glm::vec3 *_sampledDirection) const override;
};

#endif //HJRT_SAMPLER_H
