//
// Created by 何振邦 on 2019-05-12.
//

#ifndef RTTEST_UTILITY_H
#define RTTEST_UTILITY_H

#include <cmath>
#include <vector>
#include <iostream>
#include <random>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "tinyobjloader/tiny_obj_loader.h"

constexpr float ZERO = 0.0001;

struct Object;
struct HitInfo {
	float t;
	glm::vec3 hitpoint;
	glm::vec3 normal;//WARNING!!! keep normal normalized!!!
	glm::vec2 uv;
	const Object *hitobject;
	HitInfo(){t=10e10;hitobject= nullptr;};
};

struct Ray {
	glm::vec3 origin;
	glm::vec3 dir;

	Ray() = default;

	Ray(glm::vec3 _origin, glm::vec3 _dir) : origin(_origin), dir(_dir) {}
};

template<typename T>
float sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

inline glm::vec3 reflect(const glm::vec3 &dir, const glm::vec3 &normal) {
	return dir - 2 * glm::dot(dir, normal) * normal;
}

inline void gammaCorrection(glm::vec3 &color, float gamma = 2.2) {
	color.r = std::pow(color.r, 1.0 / gamma);
	color.g = std::pow(color.g, 1.0 / gamma);
	color.b = std::pow(color.b, 1.0 / gamma);
}

inline void toInt(glm::vec3 &color) {
	color.r = (color.r > 255 ? 255 : (color.r < 0 ? 0 : static_cast<int>(color.r)));
	color.g = (color.g > 255 ? 255 : (color.g < 0 ? 0 : static_cast<int>(color.g)));
	color.b = (color.b > 255 ? 255 : (color.b < 0 ? 0 : static_cast<int>(color.b)));
}
//-------------------generate random data---------------------------//
float random0_1f();

glm::vec3 randomVecUnitDisk();

glm::vec3 randomVecUnitSphere();


float fresnel(float cosine,float refractIndex);

bool refract(const glm::vec3& incidence,const glm::vec3& normal,float ni_over_nt,glm::vec3* refracted);

#endif //RTTEST_UTILITY_H
