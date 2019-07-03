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
	std::string name;
	std::shared_ptr<Material> material;

	virtual bool intersect(const Ray &ray, HitInfo *hitInfo) const = 0;

	virtual bool getAABB(AABB *box) const = 0;

	virtual bool getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const;

	virtual void transform(glm::mat4 mat);
};

struct Triangle : public Object {
	struct {
		glm::vec3 v0;
		glm::vec3 v1;
		glm::vec3 v2;
	};

	glm::vec2 uv[3];
	glm::vec3 normal;

	Triangle() = default;

	Triangle(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2);

	Triangle(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec2 _uv0, glm::vec2 _uv1, glm::vec2 _uv2);

	Triangle(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _normal) : v0(_v0), v1(_v1), v2(_v2), normal(_normal) {}

	Triangle(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _normal, glm::vec2 _uv0, glm::vec2 _uv1, glm::vec2 _uv2) : Triangle(_v0, _v1, _v2,
	                                                                                                 _normal) {
		setUVs(_uv0, _uv1, _uv2);
	}

	void setUVs(glm::vec2 _uv0, glm::vec2 _uv1, glm::vec2 _uv2);

	void transform(glm::mat4 mat)override ;

	bool intersect(const Ray &ray, HitInfo *hitInfo) const override;

	bool getAABB(AABB *box) const override;

	bool getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const override;


	glm::vec3& operator [](int index){
		index=index>2?2:index<0?0:index;
		return (&v0)[index];
	}
};

struct Sphere : public Object {
	glm::vec3 origin;
	float r;

	Sphere() = default;

	Sphere(glm::vec3 _origin, float _r) : origin(_origin), r(_r) {}

	void transform(glm::mat4 mat)override ;

	bool intersect(const Ray &ray, HitInfo *hitInfo) const override;

	bool getAABB(AABB *box) const override;

	bool getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const override;
};

struct Fog : public Object {
	std::shared_ptr<Object> boundary;
	float density;

	Fog(float _density,std::shared_ptr<Object> _boundary) : density(_density), boundary(_boundary) {};

	void transform(glm::mat4 mat) override;

	bool intersect(const Ray &ray, HitInfo *hitInfo) const override;

	bool getAABB(AABB *box) const override;

	bool getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const override;

};
#endif //RTTEST_OBJECTS_H
