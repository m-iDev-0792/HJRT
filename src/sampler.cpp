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
	//TODO. figure out why it not working
//	glm::vec3 randVec = sampleOnHemisphereCosine();//not working
//	(*_sampledDirection) = ONB::localFromW(_hitInfo.normal,randVec);
	(*_sampledDirection) = glm::normalize(_hitInfo.normal + sampleInsideSphereUniform());
	return pdf(_hitInfo, *_sampledDirection);
}

float DirectLightSampler::pdf(const HitInfo &_hitInfo, const glm::vec3 &_direction) const {
	//TODO. complete this function
	if(_direction.y==0)return 0;
	float k=(startPoint.y-_hitInfo.hitpoint.y)/_direction.y;
	glm::vec3 lightHitPoint=_hitInfo.hitpoint+k*_direction;
	glm::vec3 endPoint=startPoint+size;
	if(lightHitPoint.x<startPoint.x||lightHitPoint.x>endPoint.x)return 0;
	if(lightHitPoint.z<startPoint.z||lightHitPoint.z>endPoint.z)return 0;

	float lightCosine = std::fabs(glm::dot(lightDirection, _direction));
	if (lightCosine < 0.0001)return 0;
	glm::vec3 toLight=lightHitPoint-_hitInfo.hitpoint;
	return glm::dot(toLight, toLight) / (lightCosine * size.x * size.z);
}

float DirectLightSampler::sample(const HitInfo &_hitInfo, glm::vec3 *_sampledDirection) const {
//	bool test= random0_1f()<0.05;
	glm::vec3 point = startPoint + glm::vec3(random0_1f() * size.x, 0, random0_1f() * size.z);
	glm::vec3 toLight = point - _hitInfo.hitpoint;
//	if(test)std::cout<<"sample point: "<<point.x<<" "<<point.y<<" "<<point.z<<std::endl;
	*_sampledDirection = glm::normalize(toLight);
	float lightCosine = std::fabs(glm::dot(lightDirection, *_sampledDirection));
//	if(test)std::cout<<"lightCosine: "<<lightCosine<<std::endl;
	if (lightCosine < 0.0001)return 0;
//	if(test)std::cout<<"pdf "<<(glm::dot(toLight, toLight) / (lightCosine * size.x * size.z))<<std::endl;
	return glm::dot(toLight, toLight) / (lightCosine * size.x * size.z);

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