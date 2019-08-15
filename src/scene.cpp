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
void Scene::setSampler(std::shared_ptr<Sampler> sampler) {
	for(auto &s:objects){
		s->setSampler(sampler);
	}
}
void Scene::addShape(std::shared_ptr<Shape> shape) {
	for (auto &s:objects) {
		if (s == shape) {
			Warning("trying to add a already exist shape to scene! operation abort!\n");
			return;
		}
	}
	this->objects.push_back(shape);
}

void Scene::addShape(std::string name, std::shared_ptr<Shape> shape) {
	shape->name = name;
	addShape(shape);
}

void Scene::removeShape(std::shared_ptr<Shape> shape) {
	for (int i = 0; i < objects.size(); ++i) {
		if (objects[i] == shape) {
			objects.erase(objects.begin() + i);
			return;
		}
	}
}

void Scene::addLight(std::shared_ptr<SampleableShape> light) {
	for (auto &l:lights) {
		if (l == light) {
			Warning("trying to add a already exist light to scene! operation abort!\n");
			return;
		}
	}
	this->lights.push_back(light);
	addShape(light);

}

void Scene::addLight(std::string name, std::shared_ptr<SampleableShape> light) {
	light->name = name;
	addLight(light);
}

void Scene::removeLight(std::shared_ptr<SampleableShape> light) {
	for (int i = 0; i < lights.size(); ++i) {
		if (lights[i] == light) {
			lights.erase(lights.begin() + i);
			return;
		}
	}
	removeShape(light);
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
	for (auto &o:objects)o->prepareRendering();
	constructBVH(shutterPeriod);
}