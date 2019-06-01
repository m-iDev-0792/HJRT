//
// Created by 何振邦 on 2019-05-15.
//

#ifndef RTTEST_SCENE_H
#define RTTEST_SCENE_H

#include "bvh.h"

struct Scene {
	//TODO. tidy code
	const int MAX_BOUNCE = 25;
	vector<Object *> objects;
	BVH *bvhRoot;
	vec3 ambient;
	bool useBVH;

	bool intersect(const Ray &ray, HitInfo *hitInfo);

	vec3 shade(const Ray &ray, int depth);

	void constructBVH();
};


#endif //RTTEST_SCENE_H
