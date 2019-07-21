//
// Created by 何振邦 on 2019-05-15.
//
#define TINYOBJLOADER_IMPLEMENTATION

#include "utility.h"

float random0_1f() {
	static std::random_device seed;
	static std::mt19937 engine(seed());
	static std::uniform_real_distribution<float> dist(0, 1);
	return dist(engine);
}

glm::vec3 sampleInsideDiskUniform() {
	float theta = 2 * M_PI * random0_1f();
	float r = std::sqrt(random0_1f());
	return glm::vec3(r * std::cos(theta), r * std::sin(theta), 0);
	//Rejection sampling, Legacy code, not efficient
//	glm::vec3 p;
//	do {
//		p = 2.0f * glm::vec3(random0_1f(), random0_1f(), 0) - glm::vec3(1, 1, 0);
//	} while (glm::dot(p, p) >= 1.0);
//	return p;
}
//Sampling Sphere
glm::vec3 sampleInsideSphereUniform() {
	float r1 = random0_1f();
	float r2 = random0_1f();
	float r3 = std::cbrt(random0_1f());
	float sinTheta = 2 * std::sqrt(r2 * (1 - r2));
	return glm::vec3(r3 * std::cos(2 * M_PI * r1) * sinTheta,
	                 r3 * std::sin(2 * M_PI * r1) * sinTheta,
	                 r3 * (1 - 2 * r2));
	//Rejection sampling, Legacy code, not efficient
//	glm::vec3 p;
//	do {
//		p = 2.0f * glm::vec3(random0_1f(), random0_1f(), random0_1f()) - glm::vec3(1);
//	} while (glm::dot(p, p) >= 1.0);
//	return p;
}

glm::vec3 sampleOnSphereUniform() {
	float r1 = random0_1f();
	float r2 = random0_1f();
	float sinTheta = 2 * std::sqrt(r2 * (1 - r2));
	return glm::vec3(std::cos(2 * M_PI * r1) * sinTheta,
	                 std::sin(2 * M_PI * r1) * sinTheta,
	                 1 - 2 * r2);
}
glm::vec3 sampleOnSphereCosine() {
	float r1 = random0_1f();
	float r2 = random0_1f();
	float cosTheta = sgn(2 * r2 - 1) * std::sqrt(std::abs(2 * r2 - 1));
	float sinTheta = 2 * std::sqrt(1 - cosTheta * cosTheta);
	return glm::vec3(std::cos(2 * M_PI * r1) * sinTheta,
	                 std::sin(2 * M_PI * r1) * sinTheta,
	                 cosTheta);
}
//Sampling Hemisphere
glm::vec3 sampleOnHemisphereUniform() {
	float r1 = random0_1f();
	float r2 = random0_1f();
	float sinTheta = std::sqrt(1 - r2 * r2);
	return glm::vec3(std::cos(2 * M_PI * r1) * sinTheta,
	                 std::sin(2 * M_PI * r1) * sinTheta,
	                 r2);
}
glm::vec3 sampleOnHemisphereCosine() {
	float r1 = random0_1f();
	float r2 = random0_1f();
	float sinTheta = std::sqrt(r2);
	return glm::vec3(std::cos(2 * M_PI * r1) * sinTheta,
	                 std::sin(2 * M_PI * r1) * sinTheta,
	                 std::sqrt(1 - r2));
}

//use Schlick's approximation see https://en.wikipedia.org/wiki/Schlick%27s_approximation
float fresnel(float cosine, float refractIndex) {
	float r0 = (1 - refractIndex) / (1 + refractIndex);
	r0 *= r0;
	return r0 + (1 - r0) * std::pow(1 - cosine, 5);
}

//according to snell's law
// ni*sin(incidence)=nr*sin(refracted)
bool refract(const glm::vec3 &incidence, const glm::vec3 &normal, float ni_over_nr, glm::vec3 *refracted) {
	float _cosI = glm::dot(incidence, normal);//NOTE: _cosI= -cosI= negative cosI
	//discriminant=sinR*sinR
	float discriminant = 1 - ni_over_nr * ni_over_nr * (1 - _cosI * _cosI);
	if (discriminant > 0) {
		*refracted = ni_over_nr * (incidence - normal * _cosI) - normal * std::sqrt(discriminant);
		return true;
	} else
		return false;
}