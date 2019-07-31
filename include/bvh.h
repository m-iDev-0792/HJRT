//
// Created by 何振邦 on 2019-05-28.
//

#ifndef RTTEST_BVH_H
#define RTTEST_BVH_H


#include "objects.h"

struct BVH : Object {
	BVH() = default;

	BVH(std::vector<std::shared_ptr<Object>>::iterator begin, std::vector<std::shared_ptr<Object>>::iterator end,
	    const TimePeriod &period);

	bool intersect(const Ray &ray, HitInfo *hitInfo) const override;

	bool getAABB(const TimePeriod &period, AABB *box) const override;

private:
	std::shared_ptr<Object> leftNode;
	std::shared_ptr<Object> rightNode;
	AABB aabb;
};


#endif //RTTEST_BVH_H
