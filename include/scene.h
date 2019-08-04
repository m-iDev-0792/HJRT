//
// Created by 何振邦 on 2019-05-15.
//

#ifndef RTTEST_SCENE_H
#define RTTEST_SCENE_H

#include "bvh.h"

struct Scene {
	std::vector<std::shared_ptr<Shape>> objects;
	std::shared_ptr<BVH> bvhRoot;
	glm::vec3 ambient;
	TimePeriod shutterPeriod;

	bool useBVH;

	bool intersect(const Ray &ray, HitInfo *hitInfo) const;

	void constructBVH();

	void constructBVH(const TimePeriod &period);

	void addShape(std::shared_ptr<Shape> shape);

	void addShape(std::string name, std::shared_ptr<Shape> shape);

	void removeShape(std::shared_ptr<Shape> shape);

	//WARNNING!! integrator MUST call this funtion before performing actual rendering jobs!
	void prepareRendering();

};


#endif //RTTEST_SCENE_H
