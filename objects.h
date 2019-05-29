//
// Created by 何振邦 on 2019-05-15.
//

#ifndef RTTEST_OBJECTS_H
#define RTTEST_OBJECTS_H

#include "utility.h"
#include "aabb.h"
#include "material.h"
#include <memory>
struct Object {
	string name;
	shared_ptr<Material> material;
	virtual bool intersect(const Ray &ray, HitInfo *hitInfo)const = 0;
	virtual bool getAABB(AABB* box)const=0;
};

struct Triangle : public Object {
	vec3 v0;
	vec3 v1;
	vec3 v2;
	vec3 normal;

	Triangle() = default;

	Triangle(vec3 _v0, vec3 _v1, vec3 _v2) : v0(_v0), v1(_v1), v2(_v2) {
		vec3 E1 = this->v1 - this->v0;
		vec3 E2 = this->v2 - this->v0;
		//NOTE. we expect v0 v1 v2 are placed in counter-clock-wise
		normal = normalize(cross(E1, E2));
	}

	Triangle(vec3 _v0, vec3 _v1, vec3 _v2, vec3 _normal) : v0(_v0), v1(_v1), v2(_v2), normal(_normal) {}

	bool intersect(const Ray &ray, HitInfo *hitInfo)const override;

	bool getAABB(AABB* box)const override;
};

struct Sphere : public Object {
	vec3 origin;
	float r;

	Sphere() = default;

	Sphere(vec3 _origin, float _r) : origin(_origin), r(_r) {}

	bool intersect(const Ray &ray, HitInfo *hitInfo)const override;

	bool getAABB(AABB* box)const override;
};



#endif //RTTEST_OBJECTS_H
