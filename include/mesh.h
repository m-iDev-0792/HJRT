//
// Created by 何振邦 on 2019-06-05.
//

#ifndef HJRT_MESH_H
#define HJRT_MESH_H

#include "objects.h"
#include "bvh.h"

struct Mesh : Object {
	mat4 transform;
	AABB aabb;
	vector<shared_ptr<Triangle>> triangles;

	Mesh();

	Mesh(string path);

	void performTransform();

	bool loadMesh(string path);

	bool intersect(const Ray &ray, HitInfo *hitInfo) const override;

	bool getAABB(AABB *box) const override;
};


#endif //HJRT_MESH_H
