//
// Created by 何振邦 on 2019-05-15.
//

#ifndef RTTEST_SCENE_H
#define RTTEST_SCENE_H

#include "bvh.h"

struct Scene {
	//TODO. tidy code
	std::vector<std::shared_ptr<Object>> objects;
	std::shared_ptr<BVH> bvhRoot;
	glm::vec3 ambient;

	bool useBVH;

	bool intersect(const Ray &ray, HitInfo *hitInfo) const;

	void constructBVH();

	void constructBVH(const TimePeriod &period);
};


#endif //RTTEST_SCENE_H
