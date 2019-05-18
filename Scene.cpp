//
// Created by 何振邦 on 2019-05-15.
//

#include "Scene.h"
bool Scene::intersect(const Ray& ray,HitInfo* hitInfo){
	bool hitAnything=false;
	for(auto& obj:objects){
		bool hitThisTime=obj->intersect(ray,hitInfo);
		hitAnything=hitAnything||hitThisTime;
	}
	return hitAnything;
}
vec3 Scene::shade(const Ray& ray,int depth){
	if(depth>50)return vec3(0);
	HitInfo hitInfo;
	hitInfo.t=10e10;
	if(this->intersect(ray,&hitInfo)){
		Ray newRay;
		vec3 attenuation;
		if(hitInfo.hitobject->material->scatter(ray,hitInfo,&attenuation,&newRay))
			return hitInfo.hitobject->material->emission + attenuation*shade(newRay,depth+1);
		else
			return hitInfo.hitobject->material->emission;
	}else{
		return vec3(0.0);
	}
}