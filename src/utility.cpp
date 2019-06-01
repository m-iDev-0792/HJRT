//
// Created by 何振邦 on 2019-05-15.
//
#include "utility.h"
vec3 randomVecUnitDisk(){
	vec3 p;
	do {
		p = 2.0f*vec3(drand48(),drand48(),0) - vec3(1,1,0);
	} while (dot(p,p) >= 1.0);
	return p;
}
vec3 randomVecUnitSphere(){
	vec3 p;
	do{
		p=2.0f*vec3(drand48(),drand48(),drand48())-vec3(1);
	}while(dot(p,p)>=1.0);
	return p;
}
//use Schlick's approximation see https://en.wikipedia.org/wiki/Schlick%27s_approximation
float fresnel(float cosine,float refractIndex){
	float r0=(1-refractIndex)/(1+refractIndex);
	r0*=r0;
	return r0+(1-r0)*pow(1-cosine,5);
}

//according to snell's law
// ni*sin(incidence)=nr*sin(refracted)
bool refract(const vec3& incidence,const vec3& normal,float ni_over_nr,vec3* refracted){
	float _cosI=dot(incidence,normal);//NOTE: _cosI= -cosI= negative cosI
	//discriminant=sinR*sinR
	float discriminant=1-ni_over_nr*ni_over_nr*(1-_cosI*_cosI);
	if(discriminant>0){
		*refracted=ni_over_nr*(incidence-normal*_cosI)-normal*sqrt(discriminant);
		return true;
	} else
		return false;
}