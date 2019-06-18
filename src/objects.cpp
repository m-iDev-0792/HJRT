//
// Created by 何振邦 on 2019-05-15.
//

#include "objects.h"

bool Object::getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const {
	//do nothing
	return false;
}
void Object::transform(glm::mat4 mat) {
	//do nothing
}
void Sphere::transform(glm::mat4 mat) {
	origin=glm::vec3(mat*glm::vec4(origin,1.0f));
}
bool Sphere::getAABB(AABB *box) const {
	*box = AABB(origin - glm::vec3(r), origin + glm::vec3(r));
	return true;
}

bool Sphere::getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const {
	const glm::vec3 p = hitInfo.hitpoint - origin;
	float v = asin(p.z);
	float u = atan2(p.y, p.x);
	u = 1 - (u + M_PI) / (2 * M_PI);
	v = (v + M_PI / 2) / M_PI;
	*uvCoord = glm::vec2(u, v);
	return true;
}

bool Sphere::intersect(const Ray &ray, HitInfo *hitInfo) const {
	glm::vec3 op = origin - ray.origin; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
	float eps = 1e-4, b = glm::dot(op, ray.dir), det = b * b - glm::dot(op, op) + r * r;

	if (det < 0)
		return false;
	else
		det = sqrt(det);
	float t;

	if ((t = b - det) > eps) {
		goto HIT;
	} else if ((t = b + det) > eps) {
		goto HIT;
	} else {
		return false;
	}
	HIT:
	if (t > hitInfo->t)return false;
	hitInfo->t = t;
	hitInfo->hitpoint = ray.origin + t * ray.dir;
	hitInfo->normal = glm::normalize(hitInfo->hitpoint - origin);
	hitInfo->hitobject = this;
	return true;

}

Triangle::Triangle(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2) : v0(_v0), v1(_v1), v2(_v2) {
	glm::vec3 E1 = this->v1 - this->v0;
	glm::vec3 E2 = this->v2 - this->v0;
	//NOTE. we expect v0 v1 v2 are placed in counter-clock-wise
	normal = glm::normalize(glm::cross(E1, E2));
}

Triangle::Triangle(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec2 _uv0, glm::vec2 _uv1, glm::vec2 _uv2) : Triangle(_v0, _v1, _v2) {
	setUVs(_uv0, _uv1, _uv2);
}
void Triangle::transform(glm::mat4 mat) {
	for(int i=0;i<3;++i)
		(*this)[i]=glm::vec3(mat*glm::vec4((*this)[i],1.0f));
	normal=glm::normalize(glm::vec3(mat*glm::vec4(normal,0.0f)));
}
void Triangle::setUVs(glm::vec2 _uv0, glm::vec2 _uv1, glm::vec2 _uv2) {
	uv[0] = _uv0;
	uv[1] = _uv1;
	uv[2] = _uv2;
}

bool Triangle::getAABB(AABB *box) const {
	glm::vec3 min, max;
	for (int i = 0; i < 3; ++i) {
		min[i] = std::fmin(v0[i], std::fmin(v1[i], v2[i])) - 0.01;
		max[i] = std::fmax(v0[i], std::fmax(v1[i], v2[i])) + 0.01;
	}
	*box = AABB(min, max);
	return true;
}

bool Triangle::getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const {
	const glm::vec2 old = hitInfo.uv;
	*uvCoord = uv[0] * (1.0f - old.x - old.y) + uv[1] * old.x + uv[2] * old.y;
	return true;
}

bool Triangle::intersect(const Ray &ray, HitInfo *hitInfo) const {
	// E1
	glm::vec3 E1 = this->v1 - this->v0;
	// E2
	glm::vec3 E2 = this->v2 - this->v0;
	// P
	glm::vec3 P = glm::cross(ray.dir, E2);
	// determinant
	float det = glm::dot(P, E1);

	// keep det > 0, modify T accordingly
	glm::vec3 T;
	if (det > 0) {
		T = ray.origin - this->v0;
	} else {
		T = this->v0 - ray.origin;
		det = -det;
	}

	// If determinant is near zero, ray lies in plane of triangle
	if (det < 0.0001f)
		return false;
	float t, u, v;

	// Calculate u and make sure u <= 1
	u = glm::dot(T, P);
	if (u < 0.0f || u > det)
		return false;

	// Q
	glm::vec3 Q = glm::cross(T, E1);

	// Calculate v and make sure u + v <= 1
	v = glm::dot(ray.dir, Q);
	if (v < 0.0f || u + v > det)
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	t = glm::dot(E2, Q);

	float fInvDet = 1.0f / det;
	t *= fInvDet;
	u *= fInvDet;
	v *= fInvDet;
	//
	const float selfIntersectBias=1e-4;
	if (t > hitInfo->t || t < selfIntersectBias)return false;
	hitInfo->t = t;
	hitInfo->uv = glm::vec2(u, v);
	hitInfo->hitpoint = ray.origin + t * ray.dir;// same as :hitInfo->hitpoint=this->v0+u*E1+v*E2;
	hitInfo->normal = this->normal * sgn(glm::dot(-t * ray.dir, this->normal));
	hitInfo->hitobject = this;
	return true;
}

std::vector<std::shared_ptr<Object>> loadOBJ(std::string path,std::shared_ptr<Material> defaultMaterial) {
	std::vector<std::shared_ptr<Object>> model;
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
		return model;
	}
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

	if(defaultMaterial== nullptr)defaultMaterial=std::make_shared<Lambertian>(glm::vec3(0.8));
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

			if(hasNormal)face=std::make_shared<Triangle>(vertex[0],vertex[1],vertex[2],faceNormal);
			else face=std::make_shared<Triangle>(vertex[0],vertex[1],vertex[2]);

			if(hasTexCoord)face->setUVs(texCoord[0],texCoord[1],texCoord[2]);

			face->name=shapes[s].name+std::string("_face_")+std::to_string(f);
			model.push_back(face);
			index_offset += vertexNum;

			int mtlIndex=shapes[s].mesh.material_ids[f];
			if(mtlIndex<materials.size()&&mtlIndex>0)face->material=materialPtr[mtlIndex];
			else face->material=defaultMaterial;
		}
	}
	return model;
}