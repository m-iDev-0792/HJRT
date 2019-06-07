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

	virtual bool intersect(const Ray &ray, HitInfo *hitInfo) const = 0;

	virtual bool getAABB(AABB *box) const = 0;

	virtual bool getUV(const HitInfo &hitInfo, vec2 *uvCoord) const;
};

struct Triangle : public Object {
	struct {
		vec3 v0;
		vec3 v1;
		vec3 v2;
	};

	vec2 uv[3];
	vec3 normal;

	Triangle() = default;

	Triangle(vec3 _v0, vec3 _v1, vec3 _v2);

	Triangle(vec3 _v0, vec3 _v1, vec3 _v2, vec2 _uv0, vec2 _uv1, vec2 _uv2);

	Triangle(vec3 _v0, vec3 _v1, vec3 _v2, vec3 _normal) : v0(_v0), v1(_v1), v2(_v2), normal(_normal) {}

	Triangle(vec3 _v0, vec3 _v1, vec3 _v2, vec3 _normal, vec2 _uv0, vec2 _uv1, vec2 _uv2) : Triangle(_v0, _v1, _v2,
	                                                                                                 _normal) {
		setUVs(_uv0, _uv1, _uv2);
	}

	void setUVs(vec2 _uv0, vec2 _uv1, vec2 _uv2);

	bool intersect(const Ray &ray, HitInfo *hitInfo) const override;

	bool getAABB(AABB *box) const override;

	bool getUV(const HitInfo &hitInfo, vec2 *uvCoord) const override;

	vec3& operator [](int index){
		index=index>2?2:index<0?0:index;
		return (&v0)[index];
	}
};

struct Sphere : public Object {
	vec3 origin;
	float r;

	Sphere() = default;

	Sphere(vec3 _origin, float _r) : origin(_origin), r(_r) {}

	bool intersect(const Ray &ray, HitInfo *hitInfo) const override;

	bool getAABB(AABB *box) const override;

	bool getUV(const HitInfo &hitInfo, vec2 *uvCoord) const override;
};

vector<shared_ptr<Triangle>> loadOBJ(string path);
#endif //RTTEST_OBJECTS_H
