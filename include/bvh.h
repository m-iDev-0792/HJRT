//
// Created by 何振邦 on 2019-05-28.
//

#ifndef RTTEST_BVH_H
#define RTTEST_BVH_H


#include "shape.h"

struct BVH : Shape {
	BVH() = default;

	BVH(std::vector<std::shared_ptr<Shape>>::iterator begin, std::vector<std::shared_ptr<Shape>>::iterator end,
	    const TimePeriod &period);

	bool intersect(const Ray &ray, HitInfo *hitInfo) const override;

	bool getAABB(const TimePeriod &period, AABB *box) const override;

	void describe() const override;
private:
	std::shared_ptr<Shape> leftNode;
	std::shared_ptr<Shape> rightNode;
	AABB aabb;
	int nodeNum;
};


#endif //RTTEST_BVH_H
