//
// Created by 何振邦 on 2019-06-05.
//

#ifndef HJRT_MESH_H
#define HJRT_MESH_H

#include "geometry.h"
#include "bvh.h"
#include "material.h"

// NOTE.when use Mesh class for rendering
// call transform() first
// then call constructBVH()
struct Mesh : Shape {
	std::shared_ptr<BVH> bvhRoot;
	std::vector<std::shared_ptr<Shape>> triangles;

	Mesh();

	Mesh(const std::string &path);

	Mesh(const std::string &path, std::shared_ptr<Material> _mat);

	void constructBVH();

	bool loadMesh(const std::string &path);

	//derived functions

	void describe() const override;

	float getArea() const override;

	void setMaterial(std::shared_ptr<Material> _material) override;

	void transform(glm::mat4 mat) override;

	bool intersect(const Ray &ray, HitInfo *hitInfo) const override;

	bool getAABB(const TimePeriod &period, AABB *box) const override;

	void prepareRendering() override;
};


#endif //HJRT_MESH_H
