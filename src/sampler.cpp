//
// Created by 何振邦 on 2019-07-21.
//

#include "sampler.h"


float UniformHemisphereSampler::pdf(const HitInfo &_hitInfo, const glm::vec3 &_direction) const {
	float cosine = glm::dot(_direction, _hitInfo.normal);
	return cosine > 0 ? M_1_PI * 0.5 : 0;
}

float UniformHemisphereSampler::sample(const HitInfo &_hitInfo, glm::vec3 *_sampledDirection) const {
	glm::vec3 randVec = sampleOnHemisphereUniform();
	(*_sampledDirection) = ONB::localFromW(_hitInfo.normal,randVec);
	return pdf(_hitInfo, *_sampledDirection);
}

float CosineHemisphereSampler::pdf(const HitInfo &_hitInfo, const glm::vec3 &_direction) const {
	float cosine = glm::dot(_direction, _hitInfo.normal);
	return (cosine > 0 ? cosine : 0) * M_1_PI;
}

float CosineHemisphereSampler::sample(const HitInfo &_hitInfo, glm::vec3 *_sampledDirection) const {
	glm::vec3 randVec = sampleOnHemisphereCosine();
	(*_sampledDirection) = ONB::localFromW(_hitInfo.normal,randVec);
//	(*_sampledDirection) = glm::normalize(_hitInfo.normal + sampleOnSphereUniform());//equal to previous line of code
	return pdf(_hitInfo, *_sampledDirection);
}

float ObjectSampler::pdf(const HitInfo &_hitInfo, const glm::vec3 &_direction) const {
	return target->pdf(_hitInfo,_direction);
}

float ObjectSampler::sample(const HitInfo &_hitInfo, glm::vec3 *_sampledDirection) const {
	return target->sample(_hitInfo,_sampledDirection);
}

float MixtureSampler::pdf(const HitInfo &_hitInfo, const glm::vec3 &_direction) const {
	return p1 * sampler1->pdf(_hitInfo, _direction) + (1 - p1) * sampler2->pdf(_hitInfo, _direction);
}

float MixtureSampler::sample(const HitInfo &_hitInfo, glm::vec3 *_sampledDirection) const {
	if (p1 < 0 || p1 > 1)return 0;
	if (random0_1f() < p1) {
		float p = p1 * sampler1->sample(_hitInfo, _sampledDirection);
		return p + (1 - p) * sampler2->pdf(_hitInfo, *_sampledDirection);
	} else {
		float p = sampler2->sample(_hitInfo, _sampledDirection);
		return p + p1 * sampler1->pdf(_hitInfo, *_sampledDirection);
	}
}