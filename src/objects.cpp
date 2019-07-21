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
	isDoubleSided=false;
}

Triangle::Triangle(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec2 _uv0, glm::vec2 _uv1, glm::vec2 _uv2) : Triangle(_v0, _v1, _v2) {
	setUVs(_uv0, _uv1, _uv2);
	isDoubleSided=false;
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
	//NOTE.It may cause potential reflection&refraction problem if a non-light-source face is assigned as double-sided face, be careful!!!
	hitInfo->normal = isDoubleSided ? this->normal * sgn(glm::dot(-t * ray.dir, this->normal))
	                               : this->normal;
	hitInfo->hitobject = this;
	return true;
}

void Fog::transform(glm::mat4 mat) {
	boundary->transform(mat);
}

bool Fog::intersect(const Ray &ray, HitInfo *hitInfo) const {
	HitInfo info1, info2;
	if (boundary->intersect(ray, &info1)) {
		Ray ray2(ray.origin + (info1.t + 0.0001f) * ray.dir, ray.dir);
		if (boundary->intersect(ray2, &info2)) {
//			std::cout<<"info1 "<<info1.t<<"  info2 "<<info2.t<<std::endl;
			float distance = info2.t;
			float randomDistance = -(1 / density) * std::log(random0_1f());
			if (randomDistance > distance)return false;
			hitInfo->t = randomDistance + info1.t;
			hitInfo->normal = glm::vec3(0, 1, 0);
			hitInfo->hitobject = this;
			hitInfo->hitpoint = ray.origin + (randomDistance + info1.t) * ray.dir;
			return true;
		}
	}
	return false;
}

bool Fog::getAABB(AABB *box) const {
	return boundary->getAABB(box);
}

bool Fog::getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const {
	return false;
}