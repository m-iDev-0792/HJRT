//
// Created by 何振邦 on 2019-05-28.
//

#ifndef RTTEST_BVH_H
#define RTTEST_BVH_H


#include "objects.h"
struct BVH:Object {
	BVH()= default;
	BVH(vector<Object*>::iterator begin,vector<Object*>::iterator end);
	bool intersect(const Ray &ray, HitInfo *hitInfo)const override ;
	bool getAABB(AABB* box)const override ;
private:
	Object* leftNode;
	Object* rightNode;
	AABB aabb;
};


#endif //RTTEST_BVH_H
