//
// Created by 何振邦 on 2019-05-15.
//

#ifndef RTTEST_MATERIAL_H
#define RTTEST_MATERIAL_H

#include "utility.h"

struct Material {
	vec3 emission;
	virtual bool scatter(const Ray& ray,const HitInfo& hitInfo,vec3* attenuation,Ray* scatteredRay)const =0;
};
struct Lambertian:Material{
	vec3 albedo;
	Lambertian()= default;
	Lambertian(vec3 _albedo);
	bool scatter(const Ray& ray,const HitInfo& hitInfo,vec3* attenuation,Ray* scatteredRay)const override ;
};
struct Metal:Material{
	float fuzz;
	vec3 albedo;
	Metal()= default;
	Metal(vec3 _albedo,float _fuzz=0.2);
	bool scatter(const Ray& ray,const HitInfo& hitInfo,vec3* attenuation,Ray* scatteredRay)const override ;
};

#endif //RTTEST_MATERIAL_H
