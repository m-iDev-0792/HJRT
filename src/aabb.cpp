//
// Created by 何振邦 on 2019-05-27.
//

#include "aabb.h"
AABB AABB::getBoundingAABB(const AABB& box1,const AABB& box2){
	glm::vec3 min(std::fmin(box1.min.x,box2.min.x),std::fmin(box1.min.y,box2.min.y),std::fmin(box1.min.z,box2.min.z));
	glm::vec3 max(std::fmax(box1.max.x,box2.max.x),std::fmax(box1.max.y,box2.max.y),std::fmax(box1.max.z,box2.max.z));;
	return AABB(min,max);
}
AABB::AABB(glm::vec3 _min,glm::vec3 _max):min(_min),max(_max){

}
bool AABB::hit(const Ray& ray)const{
	float tmin=0.00001,tmax=10e10;
	for(int i=0;i<3;++i){
		if(ray.dir[i]==0.0)continue;//if ray.dir[i] is zero, we simply ignore it
		float t0=(min[i]-ray.origin[i])/ray.dir[i];
		float t1=(max[i]-ray.origin[i])/ray.dir[i];
		if(t1<t0){
			float temp=t0;
			t0=t1;
			t1=temp;
		}
		tmin=std::fmax(t0,tmin);
		tmax=std::fmin(t1,tmax);
		if(tmax<=tmin)return false;
	}
	return true;
}