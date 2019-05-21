//
// Created by 何振邦 on 2019-05-15.
//

#ifndef RTTEST_SCENE_H
#define RTTEST_SCENE_H

#include "Objects.h"

struct Scene{
	const int MAX_BOUNCE=25;
	vector<Object*> objects;
	bool intersect(const Ray& ray,HitInfo* hitInfo);
	vec3 shade(const Ray& ray,int depth);
};



#endif //RTTEST_SCENE_H
