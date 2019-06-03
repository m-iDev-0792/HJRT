//
// Created by 何振邦 on 2019-05-12.
//

#ifndef RTTEST_UTILITY_H
#define RTTEST_UTILITY_H

#include <cmath>
#include <vector>
#include <iostream>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

constexpr float ZERO = 0.0001;

struct Object;
struct HitInfo {
	float t;
	vec3 hitpoint;
	vec3 normal;
	vec2 uv;
	const Object *hitobject;
	HitInfo(){t=10e10;};
};

struct Ray {
	vec3 origin;
	vec3 dir;

	Ray() = default;

	Ray(vec3 _origin, vec3 _dir) : origin(_origin), dir(_dir) {}
};

template<typename T>
float sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

inline vec3 reflect(const vec3 &dir, const vec3 &normal) {
	return dir - 2 * dot(dir, normal) * normal;
}

inline void gammaCorrection(vec3 &color, float gamma = 2.2) {
	color.r = pow(color.r, 1.0 / gamma);
	color.g = pow(color.g, 1.0 / gamma);
	color.b = pow(color.b, 1.0 / gamma);
}

inline void toInt(vec3 &color) {
	color.r = (color.r > 255 ? 255 : (color.r < 0 ? 0 : static_cast<int>(color.r)));
	color.g = (color.g > 255 ? 255 : (color.g < 0 ? 0 : static_cast<int>(color.g)));
	color.b = (color.b > 255 ? 255 : (color.b < 0 ? 0 : static_cast<int>(color.b)));
}
//-------------------generate random data---------------------------//
float random0_1f();

vec3 randomVecUnitDisk();

vec3 randomVecUnitSphere();


float fresnel(float cosine,float refractIndex);

bool refract(const vec3& incidence,const vec3& normal,float ni_over_nt,vec3* refracted);

#endif //RTTEST_UTILITY_H