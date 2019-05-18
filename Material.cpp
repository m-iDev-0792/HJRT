//
// Created by 何振邦 on 2019-05-15.
//

#include "Material.h"
Lambertian::Lambertian(vec3 _albedo):albedo(_albedo) {
	emission=vec3(0);
}
bool Lambertian::scatter(const Ray& ray,const HitInfo& hitInfo,vec3* attenuation,Ray* scatteredRay)const{
	scatteredRay->origin=hitInfo.hitpoint;
	scatteredRay->dir=normalize(hitInfo.normal+randomVecUnitSphere());
	*attenuation=albedo;
	return true;
}
Metal::Metal(vec3 _albedo, float _fuzz):albedo(_albedo) {
	emission=vec3(0);
	fuzz=(_fuzz>1)?1:_fuzz;

}
bool Metal::scatter(const Ray& ray,const HitInfo& hitInfo,vec3* attenuation,Ray* scatteredRay)const{
	scatteredRay->origin=hitInfo.hitpoint;
	if(fuzz==0)scatteredRay->dir=normalize(reflect(ray.dir,hitInfo.normal));
	else scatteredRay->dir=normalize(reflect(ray.dir,hitInfo.normal)+fuzz*randomVecUnitSphere());
	*attenuation=albedo;
	return (dot(scatteredRay->dir,hitInfo.normal)>0);
}