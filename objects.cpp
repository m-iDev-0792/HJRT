//
// Created by 何振邦 on 2019-05-15.
//

#include "objects.h"
bool Sphere::getAABB(AABB* box)const{
	*box=AABB(origin-vec3(r),origin+vec3(r));
	return true;
}
bool Sphere::intersect(const Ray &ray, HitInfo *hitInfo)const {
	vec3 op = origin - ray.origin; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
	float eps = 1e-4, b = dot(op, ray.dir), det = b * b - dot(op, op) + r * r;

	if (det < 0)
		return false;
	else
		det = sqrt(det);
	float t;

	if ((t = b - det) > eps) {
		goto HIT;
	} else if ((t = b + det) > eps) {
		goto HIT;
	} else{
		return false;
	}
	HIT:
	if(t>hitInfo->t)return false;
	hitInfo->t=t;
	hitInfo->hitpoint = ray.origin + t * ray.dir;
	hitInfo->normal = normalize(hitInfo->hitpoint - origin);
	hitInfo->hitobject=this;
	return true;

}
bool Triangle::getAABB(AABB* box)const{
	vec3 min,max;
	for(int i=0;i<3;++i) {
		min[i] = fmin(v0[i], fmin(v1[i], v2[i]))-0.01;
		max[i] = fmax(v0[i], fmax(v1[i], v2[i]))+0.01;
	}
	*box=AABB(min,max);
	return true;
}
//TODO. test reliability
bool Triangle::intersect(const Ray &ray, HitInfo *hitInfo)const {
	// E1
	vec3 E1 = this->v1 - this->v0;
	// E2
	vec3 E2 = this->v2 - this->v0;
	// P
	vec3 P = cross(ray.dir, E2);
	// determinant
	float det = dot(P, E1);

	// keep det > 0, modify T accordingly
	vec3 T;
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
	u = dot(T, P);
	if (u < 0.0f || u > det)
		return false;

	// Q
	vec3 Q = cross(T, E1);

	// Calculate v and make sure u + v <= 1
	v = dot(ray.dir, Q);
	if (v < 0.0f || u + v > det)
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	t = dot(E2, Q);

	float fInvDet = 1.0f / det;
	t *= fInvDet;
	u *= fInvDet;
	v *= fInvDet;
	if(t>hitInfo->t||t<1e-4)return false;
	hitInfo->t=t;
	hitInfo->hitpoint = ray.origin + t * ray.dir;// same as :hitInfo->hitpoint=this->v0+u*E1+v*E2;
	hitInfo->normal = this->normal * sgn(dot(-t * ray.dir, this->normal));
	hitInfo->hitobject=this;
	return true;
}