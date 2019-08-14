//
// Created by 何振邦 on 2019-07-29.
//

#ifndef HJRT_MOVINGOBJECT_H
#define HJRT_MOVINGOBJECT_H

#include "geometry.h"

struct MovingObjectSubstitue {
	std::shared_ptr<Shape> ptr;//NOTE. our substitue.ptr doesn't have a material
	float timeTag;

	MovingObjectSubstitue() {
		timeTag = 0;
		ptr = nullptr;
	}
};

struct MovingObject {
protected:
	MovingObjectSubstitue substitute;
public:
	MovingObject() = default;

	virtual std::shared_ptr<Shape> getSubstitute(float time) const = 0;

	virtual glm::mat4 movementMat(float time) const = 0;
};

struct UniformLinearMovingObject : public MovingObject {
	glm::vec3 speed;

	glm::mat4 movementMat(float time) const override;
};

struct MovingSphere : public Sphere, public UniformLinearMovingObject {

	MovingSphere() {
		substitute.ptr = std::make_shared<Sphere>();
		substitute.timeTag = -1;
		speed = glm::vec3(0);
	}

	MovingSphere(glm::vec3 _origin, float _r, glm::vec3 _speed) : Sphere(_origin, _r) {
		substitute.ptr = std::make_shared<Sphere>(_origin, _r);
		substitute.ptr->setMaterial(std::make_shared<Lambertian>(glm::vec3(0.6)));
		Note("no material assigned, set a default lambertian material to MovingSphere\n");
		substitute.timeTag = 0;
		speed = _speed;
	}

	void setMaterial(std::shared_ptr<Material> _material) override {
		substitute.ptr->setMaterial(_material);
		material = _material;
	}

	void setSampler(std::shared_ptr<Sampler> _sampler) override {
		substitute.ptr->setSampler(_sampler);
		material->sampler = _sampler;
	}

	bool intersect(const Ray &ray, HitInfo *hitInfo) const override;

	bool getAABB(const TimePeriod &period, AABB *box) const override;

	bool getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const override;

	//derive from SampleableShape
	float pdf(const HitInfo &_hitInfo, const glm::vec3 &_direction) const override;

	float sample(const HitInfo &_hitInfo, glm::vec3 *_sampledDirection) const override;

	//derive from MovingObject
	std::shared_ptr<Shape> getSubstitute(float time) const override;

};

#endif //HJRT_MOVINGOBJECT_H
