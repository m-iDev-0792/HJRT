//
// Created by 何振邦 on 2019-05-15.
//

#include "scene.h"

void Scene::constructBVH() {
	auto objectCopy = objects;
	bvhRoot = std::make_shared<BVH>(objectCopy.begin(), objectCopy.end());
}

bool Scene::intersect(const Ray &ray, HitInfo *hitInfo)const {
	bool hitAnything = false;
	if (useBVH) {
		hitAnything = bvhRoot->intersect(ray, hitInfo);
	} else {
		for (auto &obj:objects) {
			bool hitThisTime = obj->intersect(ray, hitInfo);
			hitAnything = hitAnything || hitThisTime;
		}
	}
	return hitAnything;
}