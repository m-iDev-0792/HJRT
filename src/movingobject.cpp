//
// Created by 何振邦 on 2019-07-29.
//

#include "movingobject.h"

glm::mat4 UniformLinearMovingObject::movementMat(float time) const {
	glm::mat4 mat(1.0f);
	glm::vec3 movement = time * speed;
	return glm::translate(mat, movement);
}

std::shared_ptr<Object> MovingSphere::getSubstitute(float time) const {
	std::shared_ptr<Sphere> ptr = std::dynamic_pointer_cast<Sphere>(substitute.ptr);
	ptr->origin = transformPoint(origin, movementMat(time));
	ptr->r = this->r;
	return substitute.ptr;
}

bool MovingSphere::intersect(const Ray &ray, HitInfo *hitInfo) const {
	auto ptr = std::dynamic_pointer_cast<Sphere>(getSubstitute(ray.time));
	bool isHit = ptr->intersect(ray, hitInfo);
	if (isHit)hitInfo->hitobject = this;
	return isHit;
}

bool MovingSphere::getAABB(const TimePeriod &period, AABB *box) const {
	auto startMat = movementMat(period.start);
	auto endMat = movementMat(period.end);
	auto originStart = transformPoint(origin, startMat);
	auto originEnd = transformPoint(origin, endMat);
	*box = AABB::getBoundingAABB(AABB(originStart - glm::vec3(r), originStart + glm::vec3(r)),
	                             AABB(originEnd - glm::vec3(r), originEnd + glm::vec3(r)));
	return true;
}

bool MovingSphere::getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const {
	auto ptr = std::dynamic_pointer_cast<Sphere>(getSubstitute(hitInfo.hittime));
	return ptr->getUV(hitInfo, uvCoord);
}

float MovingSphere::pdf(const HitInfo &_hitInfo, const glm::vec3 &_direction) const {
	auto ptr = std::dynamic_pointer_cast<Sphere>(getSubstitute(_hitInfo.hittime));
	return ptr->pdf(_hitInfo, _direction);
}

float MovingSphere::sample(const HitInfo &_hitInfo, glm::vec3 *_sampledDirection) const {
	auto ptr = std::dynamic_pointer_cast<Sphere>(getSubstitute(_hitInfo.hittime));
	return ptr->sample(_hitInfo, _sampledDirection);
}

