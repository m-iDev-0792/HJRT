//
// Created by 何振邦 on 2019-06-05.
//
#include "mesh.h"

Mesh::Mesh() : transMat(1.0f) {

}

Mesh::Mesh(std::string path) {
	loadMesh(path);
}

bool Mesh::loadMesh(std::string path) {
	triangles = loadOBJ(path);
	if (triangles.empty())return false;
	else {
		name = path;
		transform(transMat);
		constructBVH();
		return true;
	}
}

void Mesh::transform(glm::mat4 mat) {
	if (mat == glm::mat4(1.0f))return;
	for (auto &t:triangles)
		t->transform(mat);
}

bool Mesh::intersect(const Ray &ray, HitInfo *hitInfo) const {
	return bvhRoot->intersect(ray, hitInfo);
}

void Mesh::constructBVH() {
	auto objectCopy = triangles;
	bvhRoot = std::make_shared<BVH>(objectCopy.begin(), objectCopy.end());
}

bool Mesh::getAABB(AABB *box) const {
	if (bvhRoot == nullptr)return false;
	bvhRoot->getAABB(box);
	return true;
}