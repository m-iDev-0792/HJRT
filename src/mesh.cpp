//
// Created by 何振邦 on 2019-06-05.
//
#include "mesh.h"

Mesh::Mesh() : transMat(1.0f) {

}

Mesh::Mesh(std::string path) {
	loadMesh(path);
}
Mesh::Mesh(std::string path, std::shared_ptr<Material> _mat){
	material=_mat;
	loadMesh(path);
}
bool Mesh::loadMesh(std::string path) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());

	if (!warn.empty()) {
		std::cout << "warn::" << warn << std::endl;
	}

	if (!err.empty()) {
		std::cerr << "err::" << err << std::endl;
	}
	if (!ret) {
		return false;
	}
	//create Material object for each obj material
	std::vector<std::shared_ptr<Material>> materialPtr;
	for(int i=0;i<materials.size();++i){
		glm::vec3 albedo(materials[i].diffuse[0],materials[i].diffuse[1],materials[i].diffuse[2]);
		glm::vec3 emission(materials[i].emission[0],materials[i].emission[1],materials[i].emission[2]);
		auto newMtl=std::make_shared<Lambertian>(albedo);
		newMtl->emission=emission;
		if(!materials[i].diffuse_texname.empty()){
			newMtl->albedoTex=std::make_shared<ImageTexture>(materials[i].diffuse_texname);
		}
		if(!materials[i].emissive_texname.empty()){
			newMtl->emissionTex=std::make_shared<ImageTexture>(materials[i].emissive_texname);
		}
		materialPtr.push_back(newMtl);
	}
	//if mesh doesn't have a material yet then use default material
	if(material== nullptr)material=std::make_shared<Lambertian>(glm::vec3(0.8));
	triangles.clear();
	//Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		size_t index_offset = 0;

		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			int vertexNum = shapes[s].mesh.num_face_vertices[f];
			glm::vec3 vertex[3], normal[3];
			glm::vec2 texCoord[3];
			bool hasNormal = true, hasTexCoord = true;

			// Loop over vertices in the face.
			for (size_t v = 0; v < vertexNum; v++) {
				if(v>=3)break;
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

				vertex[v].x = attrib.vertices[3 * idx.vertex_index + 0];
				vertex[v].y = attrib.vertices[3 * idx.vertex_index + 1];
				vertex[v].z = attrib.vertices[3 * idx.vertex_index + 2];
				if (3 * idx.normal_index >= attrib.normals.size())hasNormal = false;
				else {
					normal[v].x = attrib.normals[3 * idx.normal_index + 0];
					normal[v].y = attrib.normals[3 * idx.normal_index + 1];
					normal[v].z = attrib.normals[3 * idx.normal_index + 2];
				}
				if (2 * idx.texcoord_index >= attrib.texcoords.size())hasTexCoord = false;
				else {
					texCoord[v].x = attrib.texcoords[2 * idx.texcoord_index + 0];
					texCoord[v].y = attrib.texcoords[2 * idx.texcoord_index + 1];
				}

			}
			std::shared_ptr<Triangle> face;
			glm::vec3 faceNormal(0);
			for(auto& n:normal)faceNormal+=n;
			faceNormal*=1/3.0f;

			if(hasNormal)face=std::make_shared<Triangle>(vertex[0],vertex[1],vertex[2],normal[0]);
			else{
				face=std::make_shared<Triangle>(vertex[0],vertex[1],vertex[2]);
			}

			if(hasTexCoord)face->setUVs(texCoord[0],texCoord[1],texCoord[2]);

			face->name=shapes[s].name+std::string("_face_")+std::to_string(f);
			triangles.push_back(face);
			index_offset += vertexNum;

			int mtlIndex=shapes[s].mesh.material_ids[f];
			if(mtlIndex<materials.size()&&mtlIndex>0)face->material=materialPtr[mtlIndex];
			else face->material=material;
		}
	}

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