//
// Created by 何振邦 on 2019-05-12.
//

#ifndef RTTEST_UTILITY_H
#define RTTEST_UTILITY_H


#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "tinyobjloader/tiny_obj_loader.h"
#include "constants.h"
#include <vector>
#include <iostream>
#include <random>
#include <string>

template<typename T>
float sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

std::ostream& operator <<(std::ostream &s,const glm::vec2 vec);

std::ostream& operator <<(std::ostream &s,const glm::vec3 vec);

inline glm::vec3 transformPoint(const glm::vec3 &point, const glm::mat4 &mat) {
	return glm::vec3(mat * glm::vec4(point, 1.0f));
}

inline glm::vec3 transformVector(const glm::vec3 &vec, const glm::mat4 &mat) {
	return glm::vec3(mat * glm::vec4(vec, 0.0f));
}

inline glm::vec3 deNanInf(const glm::vec3 &data) {
	glm::vec3 temp = data;
	for (int i = 0; i < 3; ++i) {
		if (isnan(temp[i]))temp[i] = 0;
		else if (isinf(temp[i]))temp[i] = 1;
	}
	return temp;
}

inline glm::vec3 reflect(const glm::vec3 &dir, const glm::vec3 &normal) {
	return dir - 2 * glm::dot(dir, normal) * normal;
}

inline void gammaCorrection(glm::vec3 &color, float gamma = 2.2) {
	color.r = std::pow(color.r, 1.0 / gamma);
	color.g = std::pow(color.g, 1.0 / gamma);
	color.b = std::pow(color.b, 1.0 / gamma);
}

//-------------------generate random data---------------------------//
float random0_1f();

glm::vec3 sampleInsideDiskUniform();

glm::vec3 sampleInsideSphereUniform();

glm::vec3 sampleOnSphereUniform();

glm::vec3 sampleOnSphereCosine();

glm::vec3 sampleOnHemisphereUniform();

glm::vec3 sampleOnHemisphereCosine();

glm::vec3 sampleInsideTriangleUniform(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2);

float fresnel(float cosine, float refractIndex);

bool refract(const glm::vec3 &incidence, const glm::vec3 &normal, float ni_over_nt, glm::vec3 *refracted);

#endif //RTTEST_UTILITY_H
