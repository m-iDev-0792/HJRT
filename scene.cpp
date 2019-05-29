//
// Created by 何振邦 on 2019-05-15.
//

#include "scene.h"

void Scene::constructBVH() {
	auto objectCopy = objects;
	bvhRoot = new BVH(objectCopy.begin(), objectCopy.end());

}

bool Scene::intersect(const Ray &ray, HitInfo *hitInfo) {
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

vec3 Scene::shade(const Ray &ray, int depth) {
	if (depth > MAX_BOUNCE)return ambient;
	HitInfo hitInfo;
	if (this->intersect(ray, &hitInfo)) {
		if (depth > 5) {
			const vec3 &f = hitInfo.hitobject->material->albedo;
			float maxContribution = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y : f.z;
			if (drand48() > maxContribution)return hitInfo.hitobject->material->emission;
		}
		Ray newRay;
		vec3 attenuation;
		if (hitInfo.hitobject->material->scatter(ray, hitInfo, &attenuation, &newRay)) {
			return hitInfo.hitobject->material->emission + attenuation * shade(newRay, depth + 1);
		} else
			return hitInfo.hitobject->material->emission;
	} else {
//		if(depth==1)cout<<"no hit at first hit"<<endl;
		return ambient;
	}
}