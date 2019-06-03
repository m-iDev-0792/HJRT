//
// Created by 何振邦 on 2019-05-27.
//

#ifndef RTTEST_AABB_H
#define RTTEST_AABB_H

#include "utility.h"
using namespace glm;
using namespace std;
struct AABB {
	vec3 min;
	vec3 max;
	AABB()= default;
	AABB(vec3 _min,vec3 _max);
	bool hit(const Ray& ray)const;
	static AABB getBoundingAABB(const AABB& box1,const AABB& box2);
};


#endif //RTTEST_AABB_H