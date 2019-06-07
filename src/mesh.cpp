//
// Created by 何振邦 on 2019-06-05.
//
#include "mesh.h"
Mesh::Mesh():transform(1.0f){

}
Mesh::Mesh(string path) {
	loadMesh(path);
}
bool Mesh::loadMesh(string path){
	triangles=loadOBJ(path);
	return !triangles.empty();
}
void Mesh::performTransform() {

}
bool Mesh::intersect(const Ray &ray, HitInfo *hitInfo)const{
	return true;
}
bool Mesh::getAABB(AABB *box) const{
	*box=aabb;
	return true;
}