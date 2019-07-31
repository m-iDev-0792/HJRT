//
// Created by 何振邦 on 2019-05-27.
//

#ifndef RTTEST_AABB_H
#define RTTEST_AABB_H

#include "ray.h"
#include <cmath>

struct AABB {
	glm::vec3 min;
	glm::vec3 max;

	AABB() = default;

	AABB(glm::vec3 _min, glm::vec3 _max);

	bool hit(const Ray &ray) const;

	static AABB getBoundingAABB(const AABB &box1, const AABB &box2);
};


#endif //RTTEST_AABB_H
