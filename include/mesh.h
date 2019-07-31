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
struct Mesh : Object {
	glm::mat4 transMat;//WARNING tranMat actually doesn't work too much
	std::shared_ptr<BVH> bvhRoot;
	std::vector<std::shared_ptr<Object>> triangles;

	Mesh();

	Mesh(const std::string &path);

	Mesh(const std::string &path, std::shared_ptr<Material> _mat);

	void constructBVH();

	bool loadMesh(const std::string &path);

	void transform(glm::mat4 mat) override;

	bool intersect(const Ray &ray, HitInfo *hitInfo) const override;

	bool getAABB(const TimePeriod &period, AABB *box) const override;
};


#endif //HJRT_MESH_H
