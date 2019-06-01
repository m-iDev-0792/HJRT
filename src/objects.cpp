//
// Created by 何振邦 on 2019-05-15.
//

#include "objects.h"
bool Object::getUV(const HitInfo &hitInfo, vec2 *uvCoord) const{
	return false;
}
bool Sphere::getAABB(AABB* box)const{
	*box=AABB(origin-vec3(r),origin+vec3(r));
	return true;
}
bool Sphere::getUV(const HitInfo &hitInfo, vec2 *uvCoord) const{
	const vec3 p=hitInfo.hitpoint-origin;
	float v=asin(p.z);
	float u=atan2(p.y,p.x);
	u=1-(u+M_PI)/(2*M_PI);
	v=(v+M_PI/2)/M_PI;
	*uvCoord=vec2(u,v);
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
Triangle::Triangle(vec3 _v0, vec3 _v1, vec3 _v2) : v0(_v0), v1(_v1), v2(_v2) {
	vec3 E1 = this->v1 - this->v0;
	vec3 E2 = this->v2 - this->v0;
	//NOTE. we expect v0 v1 v2 are placed in counter-clock-wise
	normal = normalize(cross(E1, E2));
}
Triangle::Triangle(vec3 _v0, vec3 _v1, vec3 _v2,vec2 _uv0,vec2 _uv1,vec2 _uv2):Triangle(_v0, _v1, _v2){
	setUVs(_uv0,_uv1,_uv2);
}
void Triangle::setUVs(vec2 _uv0, vec2 _uv1, vec2 _uv2) {
	uv[0]=_uv0;
	uv[1]=_uv1;
	uv[2]=_uv2;
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
bool Triangle::getUV(const HitInfo &hitInfo, vec2 *uvCoord) const{
	const vec2 old=hitInfo.uv;
	*uvCoord=uv[0]*(1.0f-old.x-old.y)+uv[1]*old.x+uv[2]*old.y;
	return true;
}
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
	hitInfo->uv=vec2(u,v);
	hitInfo->hitpoint = ray.origin + t * ray.dir;// same as :hitInfo->hitpoint=this->v0+u*E1+v*E2;
	hitInfo->normal = this->normal * sgn(dot(-t * ray.dir, this->normal));
	hitInfo->hitobject=this;
	return true;
}
