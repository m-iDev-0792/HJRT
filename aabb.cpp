//
// Created by 何振邦 on 2019-05-27.
//

#include "aabb.h"
AABB AABB::getBoundingAABB(const AABB& box1,const AABB& box2){
	vec3 min(fmin(box1.min.x,box2.min.x),fmin(box1.min.y,box2.min.y),fmin(box1.min.z,box2.min.z));
	vec3 max(fmax(box1.max.x,box2.max.x),fmax(box1.max.y,box2.max.y),fmax(box1.max.z,box2.max.z));;
	return AABB(min,max);
}
AABB::AABB(vec3 _min,vec3 _max):min(_min),max(_max){

}
bool AABB::hit(const Ray& ray)const{
	float tmin=0.00001,tmax=10e10;
	for(int i=0;i<3;++i){
		if(ray.dir[i]==0.0)continue;//if ray.dir[i] is zero, we simply ignore it
		float t0=(min[i]-ray.origin[i])/ray.dir[i];
		float t1=(max[i]-ray.origin[i])/ray.dir[i];
		if(t1<t0)
			swap(t0,t1);
		tmin=fmax(t0,tmin);
		tmax=fmin(t1,tmax);
		if(tmax<=tmin)return false;
	}
	return true;
}