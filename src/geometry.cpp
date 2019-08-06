//
// Created by 何振邦 on 2019-07-26.
//

#include "geometry.h"
#include "onb.h"

//----------------------------------
//           Sphere
//----------------------------------
void Sphere::describe() const {
	std::cout << "Sphere " << name << ", r=" << r << ", origin=" << origin << std::endl;
}

void Sphere::transform(glm::mat4 mat) {
	origin = glm::vec3(mat * glm::vec4(origin, 1.0f));
}

bool Sphere::getAABB(const TimePeriod &period, AABB *box) const {
	*box = AABB(origin - glm::vec3(r), origin + glm::vec3(r));
	return true;
}

bool Sphere::getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const {
	const glm::vec3 p = hitInfo.hitpoint - origin;
	float v = asin(p.z);
	float u = atan2(p.y, p.x);
	u = 1 - (u + C_PI) / (2 * C_PI);
	v = (v + C_PI_OVER_TWO) / C_PI;
	*uvCoord = glm::vec2(u, v);
	return true;
}

float Sphere::getArea() const {
	return C_FOUR_PI * r * r;
}

bool Sphere::intersect(const Ray &ray, HitInfo *hitInfo) const {
	glm::vec3 op = origin - ray.origin; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
	float b = glm::dot(op, ray.dir), det = b * b - glm::dot(op, op) + r * r;

	if (det < 0)
		return false;
	else
		det = sqrt(det);
	float t;

	if ((t = b - det) > ray.tMin) {
		goto HIT;
	} else if ((t = b + det) > ray.tMin) {
		goto HIT;
	} else {
		return false;
	}
	HIT:
	if (t > hitInfo->t || t > ray.tMax)return false;
	hitInfo->t = t;
	hitInfo->hitpoint = ray.origin + t * ray.dir;
	hitInfo->normal = glm::normalize(hitInfo->hitpoint - origin);
	hitInfo->hitobject = this;
	hitInfo->hittime = ray.time;
	return true;

}

float Sphere::pdf(const HitInfo &_hitInfo, const glm::vec3 &_direction) const {
	HitInfo info;
	Ray ray(_hitInfo.hitpoint, _direction, _hitInfo.hittime);
	if (this->intersect(ray, &info)) {
		glm::vec3 p2c = origin - _hitInfo.hitpoint;
		float cosineMax = std::sqrt(1 - r * r / (glm::dot(p2c, p2c)));
		float solidAngle = C_TWO_PI * (1 - cosineMax);
		return 1 / solidAngle;
	} else
		return 0;
}

float Sphere::sample(const HitInfo &_hitInfo, glm::vec3 *_sampledDirection) const {
	float r1 = random0_1f(), r2 = random0_1f();
	glm::vec3 p2c = origin - _hitInfo.hitpoint;
	float cosineMax = std::sqrt(1 - r * r / (glm::dot(p2c, p2c)));
	float z = 1 + r2 * (cosineMax - 1);
	float sinTheta = std::sqrt(1 - z * z);
	float phi = C_TWO_PI * r1;
	float x = std::cos(phi) * sinTheta;
	float y = std::sin(phi) * sinTheta;
	*_sampledDirection = ONB::localFromW(_hitInfo.normal, glm::vec3(x, y, z));
	float solidAngle = C_TWO_PI * (1 - cosineMax);
	return 1 / solidAngle;
}

//----------------------------------
//           Triangle
//----------------------------------
Triangle::Triangle(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2) : v0(_v0), v1(_v1), v2(_v2) {
	glm::vec3 E1 = this->v1 - this->v0;
	glm::vec3 E2 = this->v2 - this->v0;
	//NOTE. we expect v0 v1 v2 are placed in counter-clock-wise
	normal = glm::normalize(glm::cross(E1, E2));
	isDoubleSided = false;
}

Triangle::Triangle(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec2 _uv0, glm::vec2 _uv1, glm::vec2 _uv2)
		: Triangle(_v0, _v1, _v2) {
	setUVs(_uv0, _uv1, _uv2);
	isDoubleSided = false;
}

void Triangle::describe() const {
	std::cout << "Triangle " << name << ", v0=" << v0 << ", v1=" << v1 << ", v2=" << v2 << std::endl;
}

void Triangle::transform(glm::mat4 mat) {
	for (int i = 0; i < 3; ++i)
		(*this)[i] = glm::vec3(mat * glm::vec4((*this)[i], 1.0f));
	normal = glm::normalize(glm::vec3(mat * glm::vec4(normal, 0.0f)));
}

void Triangle::setUVs(glm::vec2 _uv0, glm::vec2 _uv1, glm::vec2 _uv2) {
	uv[0] = _uv0;
	uv[1] = _uv1;
	uv[2] = _uv2;
}

bool Triangle::getAABB(const TimePeriod &period, AABB *box) const {
	glm::vec3 min, max;
	for (int i = 0; i < 3; ++i) {
		min[i] = std::fmin(v0[i], std::fmin(v1[i], v2[i])) - 0.01;
		max[i] = std::fmax(v0[i], std::fmax(v1[i], v2[i])) + 0.01;
	}
	*box = AABB(min, max);
	return true;
}

bool Triangle::getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const {
	//TODO. test modification stability, we already done this in **intersection** function
	*uvCoord = hitInfo.uv;
//	const glm::vec2 old = hitInfo.uv;
//	*uvCoord = uv[0] * (1.0f - old.x - old.y) + uv[1] * old.x + uv[2] * old.y;
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

	if (t > hitInfo->t || t < ray.tMin || t > ray.tMax)return false;
	hitInfo->t = t;
	hitInfo->uv = uv[0] * (1.0f - u - v) + uv[1] * u + uv[2] * v;//glm::vec2(u, v); //modified in 2019-7-25
	hitInfo->hitpoint = ray.origin + t * ray.dir;// same as :hitInfo->hitpoint=this->v0+u*E1+v*E2;
	//NOTE.It may cause potential reflection&refraction problem if a non-light-source face is assigned as double-sided face, be careful!!!
	hitInfo->normal = isDoubleSided ? this->normal * sgn(glm::dot(-t * ray.dir, this->normal))
	                                : this->normal;
	hitInfo->hitobject = this;
	hitInfo->hittime = ray.time;
	return true;
}

float Triangle::getArea() const {
	glm::vec3 l1 = v1 - v0;
	glm::vec3 l2 = v2 - v0;
	return glm::length(glm::cross(l2, l1)) / 2;
}

float Triangle::pdf(const HitInfo &_hitInfo, const glm::vec3 &_direction) const {
	Ray ray(_hitInfo.hitpoint, _direction, _hitInfo.hittime);
	HitInfo info;
	if (!this->intersect(ray, &info))return 0;
	float cosine = std::fabs(glm::dot(normal, _direction));
	if (cosine < 0.00001)return 0;
	return info.t * info.t / (cosine * getArea());
}

float Triangle::sample(const HitInfo &_hitInfo, glm::vec3 *_sampledDirection) const {
	//generate random point, random point picking algorithm from https://blog.csdn.net/noahzuo/article/details/52886447
	glm::vec3 point = sampleInsideTriangleUniform(v0, v1, v2);
	//direction
	glm::vec3 dis = point - _hitInfo.hitpoint;
	*_sampledDirection = glm::normalize(dis);
	//calculate sampling pdf
	float cosine = std::fabs(glm::dot(normal, *_sampledDirection));
	if (cosine < 0.00001)return 0;
	return glm::dot(dis, dis) / (cosine * getArea());
}

//----------------------------------
//           Plane
//----------------------------------
//    Plane Vertices Layout
//     v3----<----v2
//                |
//                ^
//                |
//     v0---->----v1
Plane::Plane(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _v3) {
	glm::vec2 defulatUV[4] = {glm::vec2(0), glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(0, 1)};
	triangles[0] = Triangle(_v0, _v1, _v3, defulatUV[0], defulatUV[1], defulatUV[3]);
	triangles[1] = Triangle(_v1, _v2, _v3, defulatUV[1], defulatUV[2], defulatUV[3]);
}

Plane::Plane(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _v3, glm::vec3 _normal) {
	glm::vec2 defulatUV[4] = {glm::vec2(0), glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(0, 1)};
	triangles[0] = Triangle(_v0, _v1, _v3, _normal, defulatUV[0], defulatUV[1], defulatUV[3]);
	triangles[1] = Triangle(_v1, _v2, _v3, _normal, defulatUV[1], defulatUV[2], defulatUV[3]);
}

Plane::Plane(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _v3, glm::vec3 _uv0, glm::vec3 _uv1, glm::vec3 _uv2,
             glm::vec3 _uv3) {
	triangles[0] = Triangle(_v0, _v1, _v3, _uv0, _uv1, _uv3);
	triangles[1] = Triangle(_v1, _v2, _v3, _uv1, _uv2, _uv3);
}

Plane::Plane(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _v3, glm::vec3 _normal, glm::vec3 _uv0,
             glm::vec3 _uv1, glm::vec3 _uv2, glm::vec3 _uv3) {
	triangles[0] = Triangle(_v0, _v1, _v3, _normal, _uv0, _uv1, _uv3);
	triangles[1] = Triangle(_v1, _v2, _v3, _normal, _uv1, _uv2, _uv3);
}

void Plane::describe() const {
	auto &t1 = triangles[0];
	auto &t2 = triangles[1];
	std::cout << "Plane " << name << "\n v0=" << t1.v0 << ", v1=" << t2.v0 << "\nv2=" << t2.v1 << ", v3=" << t2.v2
	          << std::endl;
}

void Plane::transform(glm::mat4 mat) {
	triangles[0].transform(mat);
	triangles[1].transform(mat);
}

bool Plane::getAABB(const TimePeriod &period, AABB *box) const {
	glm::vec3 min, max;
	for (int i = 0; i < 3; ++i) {
		//------------------------------v0---------------------------v1----------------------------v2------------------v3-------------
		min[i] = std::fmin(triangles[0].v0[i],
		                   std::fmin(triangles[1].v0[i], std::fmin(triangles[1].v1[i], triangles[1].v2[i]))) - 0.01;
		max[i] = std::fmax(triangles[0].v0[i],
		                   std::fmax(triangles[1].v0[i], std::fmax(triangles[1].v1[i], triangles[1].v2[i]))) + 0.01;
	}
	*box = AABB(min, max);
	return true;
}

void Plane::setUVs(glm::vec2 _uv0, glm::vec2 _uv1, glm::vec2 _uv2, glm::vec2 _uv3) {
	triangles[0].setUVs(_uv0, _uv1, _uv3);
	triangles[1].setUVs(_uv1, _uv2, _uv3);
}

bool Plane::intersect(const Ray &ray, HitInfo *hitInfo) const {
	return triangles[0].intersect(ray, hitInfo) || triangles[1].intersect(ray, hitInfo);
}

bool Plane::getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const {
	*uvCoord = hitInfo.uv;
	return true;
}

float Plane::getArea() const {
	return triangles[0].getArea() + triangles[1].getArea();
}

float Plane::pdf(const HitInfo &_hitInfo, const glm::vec3 &_direction) const {
	//actually one of the pdfs must be ZERO, real pdf value is 0.5*triangles[X].pdf
	return 0.5f * (triangles[0].pdf(_hitInfo, _direction) + triangles[1].pdf(_hitInfo, _direction));
}

float Plane::sample(const HitInfo &_hitInfo, glm::vec3 *_sampledDirection) const {
	glm::vec3 v0 = triangles[0].v0, v1 = triangles[1].v0, v3 = triangles[1].v2;
	glm::vec3 l1 = v1 - v0, l2 = v3 - v0;
	glm::vec3 point = v0 + random0_1f() * l1 + random0_1f() * l2;
	//direction
	glm::vec3 dis = point - _hitInfo.hitpoint;
	*_sampledDirection = glm::normalize(dis);
	//calculate sampling pdf
	float cosine = std::fabs(glm::dot(triangles[0].normal, *_sampledDirection));
	if (cosine < 0.00001)return 0;
	return pdf(_hitInfo, *_sampledDirection);
}

//----------------------------------
//           Fog
//----------------------------------
void Fog::describe() const {
	std::cout << "Fog " << name << " boundary: ";
	boundary->describe();
}

void Fog::transform(glm::mat4 mat) {
	boundary->transform(mat);
}

bool Fog::intersect(const Ray &ray, HitInfo *hitInfo) const {
	HitInfo info1, info2;
	if (boundary->intersect(ray, &info1)) {
		Ray ray2(ray.origin + (info1.t + 0.0001f) * ray.dir, ray.dir, ray.time, ray.tMin, ray.tMax);
		if (boundary->intersect(ray2, &info2)) {
			float distance = info2.t;
			float randomDistance = -(1 / density) * std::log(random0_1f());
			if (randomDistance > distance)return false;
			hitInfo->t = randomDistance + info1.t;
			hitInfo->normal = glm::vec3(0, 1, 0);
			hitInfo->hitobject = this;
			hitInfo->hitpoint = ray.origin + (randomDistance + info1.t) * ray.dir;
			hitInfo->hittime = ray.time;
			return true;
		}
	}
	return false;
}

bool Fog::getAABB(const TimePeriod &period, AABB *box) const {
	return boundary->getAABB(period, box);
}

bool Fog::getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const {
	*uvCoord = hitInfo.uv;
	return false;
}

float Fog::getArea() const {
	return boundary->getArea();
}