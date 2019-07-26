//
// Created by 何振邦 on 2019-05-15.
//

#ifndef RTTEST_OBJECTS_H
#define RTTEST_OBJECTS_H

#include "utility.h"
#include "aabb.h"
#include <memory>
struct Material;
struct Object {
	std::string name;
	std::shared_ptr<Material> material;

	virtual bool intersect(const Ray &ray, HitInfo *hitInfo) const = 0;

	virtual bool getAABB(AABB *box) const = 0;

	virtual bool getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const;

	virtual void transform(glm::mat4 mat);
};

struct SampleableObject : public Object {
	virtual float pdf(const HitInfo &_hitInfo, const glm::vec3 &_direction) const = 0;

	virtual float sample(const HitInfo &_hitInfo, glm::vec3 *_sampledDirection) const = 0;
};


#endif //RTTEST_OBJECTS_H
