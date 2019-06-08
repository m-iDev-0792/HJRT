//
// Created by 何振邦 on 2019-05-15.
//

#include "scene.h"

void Scene::constructBVH() {
	auto objectCopy = objects;
	bvhRoot = std::make_shared<BVH>(objectCopy.begin(), objectCopy.end());
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

glm::vec3 Scene::shade(const Ray &ray, int depth) {
	if (depth > MAX_BOUNCE)return ambient;
	HitInfo hitInfo;
	if (this->intersect(ray, &hitInfo)) {
		glm::vec2 uv;
		bool hasUV = hitInfo.hitobject->getUV(hitInfo, &uv);
		hitInfo.uv = uv;
		glm::vec3 emission = hitInfo.hitobject->material->emissionTex == nullptr ?
		                hitInfo.hitobject->material->emission :
		                hitInfo.hitobject->material->emissionTex->getColor(uv);

		if (depth > 3) {
			const glm::vec3 &f = hitInfo.hitobject->material->albedoTex== nullptr?
			                hitInfo.hitobject->material->albedo:
			                hitInfo.hitobject->material->albedoTex->getColor(uv);
			float maxContribution = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y : f.z;
			if (random0_1f() > maxContribution)return emission;
		}
		Ray newRay;
		glm::vec3 attenuation;
		if (hitInfo.hitobject->material->scatter(ray, hitInfo, &attenuation, &newRay)) {
			return emission + attenuation * shade(newRay, depth + 1);
		} else
			return emission;
	} else {
		return ambient;
	}
}