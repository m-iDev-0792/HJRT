//
// Created by 何振邦 on 2019-05-15.
//

#include "scene.h"

void Scene::constructBVH() {
	if (!useBVH)return;
	auto objectCopy = objects;
	bvhRoot = std::make_shared<BVH>(objectCopy.begin(), objectCopy.end(), TimePeriod(0, 0));
}

void Scene::constructBVH(const TimePeriod &period) {
	if (!useBVH)return;
	auto objectCopy = objects;
	bvhRoot = std::make_shared<BVH>(objectCopy.begin(), objectCopy.end(), period);
}

void Scene::addShape(std::shared_ptr<Shape> shape) {
	this->objects.push_back(shape);
}

void Scene::addShape(std::string name, std::shared_ptr<Shape> shape) {
	shape->name = name;
	this->objects.push_back(shape);
}

void Scene::removeShape(std::shared_ptr<Shape> shape) {
	for (int i = 0; i < objects.size(); ++i) {
		if (objects[i] == shape) {
			objects.erase(objects.begin() + i);
			return;
		}
	}
}

bool Scene::intersect(const Ray &ray, HitInfo *hitInfo) const {
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

void Scene::prepareRendering() {
	for(auto& o:objects)o->prepareRendering();
	constructBVH(shutterPeriod);
}