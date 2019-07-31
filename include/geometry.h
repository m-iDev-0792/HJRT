//
// Created by 何振邦 on 2019-07-26.
//

#ifndef HJRT_GEOMETRY_H
#define HJRT_GEOMETRY_H

#include "objects.h"

struct Triangle : public SampleableObject {
	struct {
		glm::vec3 v0;
		glm::vec3 v1;
		glm::vec3 v2;
	};

	glm::vec2 uv[3];
	glm::vec3 normal;
	//NOTE.It may cause potential reflection&refraction problem if a non-light-source face is assigned as double-sided face, be careful!!!
	bool isDoubleSided;

	Triangle() { isDoubleSided = false; }

	Triangle(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2);

	Triangle(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec2 _uv0, glm::vec2 _uv1, glm::vec2 _uv2);

	Triangle(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _normal) : v0(_v0), v1(_v1), v2(_v2), normal(_normal) {
		isDoubleSided = false;
	}

	Triangle(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _normal, glm::vec2 _uv0, glm::vec2 _uv1,
	         glm::vec2 _uv2) : Triangle(_v0, _v1, _v2, _normal) {
		setUVs(_uv0, _uv1, _uv2);
		isDoubleSided = false;
	}

	void setUVs(glm::vec2 _uv0, glm::vec2 _uv1, glm::vec2 _uv2);

	void transform(glm::mat4 mat) override;

	bool intersect(const Ray &ray, HitInfo *hitInfo) const override;

	bool getAABB(const TimePeriod &period, AABB *box) const override;

	bool getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const override;

	float getArea() const;

	//derive from SampleableObject
	float pdf(const HitInfo &_hitInfo, const glm::vec3 &_direction) const override;

	float sample(const HitInfo &_hitInfo, glm::vec3 *_sampledDirection) const override;

	glm::vec3 &operator[](int index) {
		index = index > 2 ? 2 : index < 0 ? 0 : index;
		return (&v0)[index];
	}
};

struct Plane : public SampleableObject {
	Triangle triangles[2];

	Plane() = default;

	Plane(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _v3);

	Plane(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _v3, glm::vec3 _normal);

	Plane(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _v3, glm::vec3 _uv0, glm::vec3 _uv1, glm::vec3 _uv2,
	      glm::vec3 _uv3);

	Plane(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _v3, glm::vec3 _normal, glm::vec3 _uv0, glm::vec3 _uv1,
	      glm::vec3 _uv2, glm::vec3 _uv3);

	void setMaterial(std::shared_ptr<Material> _material){
		triangles[0].material=triangles[1].material=material=_material;
	}

	void setUVs(glm::vec2 _uv0, glm::vec2 _uv1, glm::vec2 _uv2, glm::vec2 _uv3);

	void transform(glm::mat4 mat) override;

	bool intersect(const Ray &ray, HitInfo *hitInfo) const override;

	bool getAABB(const TimePeriod &period, AABB *box) const override;

	bool getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const override;

	float getArea() const;

	//derive from SampleableObject
	float pdf(const HitInfo &_hitInfo, const glm::vec3 &_direction) const override;

	float sample(const HitInfo &_hitInfo, glm::vec3 *_sampledDirection) const override;
};

struct Sphere : public SampleableObject {
	glm::vec3 origin;
	float r;

	Sphere() = default;

	Sphere(glm::vec3 _origin, float _r) : origin(_origin), r(_r) {}

	void transform(glm::mat4 mat) override;

	bool intersect(const Ray &ray, HitInfo *hitInfo) const override;

	bool getAABB(const TimePeriod &period, AABB *box) const override;

	bool getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const override;

	//derive from SampleableObject
	float pdf(const HitInfo &_hitInfo, const glm::vec3 &_direction) const override;

	float sample(const HitInfo &_hitInfo, glm::vec3 *_sampledDirection) const override;
};

struct Fog : public Object {
	std::shared_ptr<Object> boundary;
	float density;

	Fog(float _density, std::shared_ptr<Object> _boundary) : density(_density), boundary(_boundary) {};

	void transform(glm::mat4 mat) override;

	bool intersect(const Ray &ray, HitInfo *hitInfo) const override;

	bool getAABB(const TimePeriod &period, AABB *box) const override;

	bool getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const override;

};
#endif //HJRT_GEOMETRY_H
