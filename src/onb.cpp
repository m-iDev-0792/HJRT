//
// Created by 何振邦 on 2019-07-19.
//
#include "onb.h"

ONB::ONB(glm::vec3 _w) {
	w = glm::normalize(_w);
	glm::vec3 a;
	if (std::fabs(w.x) > 0.9)
		a = glm::vec3(0, 1, 0);
	else
		a = glm::vec3(1, 0, 0);
	v = glm::normalize(glm::cross(w, a));
	u = glm::normalize(glm::cross(w, v));
}

ONB::ONB(glm::vec3 _u, glm::vec3 _v, glm::vec3 _w) : u(_u), v(_v), w(_w) {

}

glm::vec3 ONB::local(const glm::vec3 &uvw) const {
	return u * uvw.x + v * uvw.y + w * uvw.z;
}

glm::vec3 ONB::local(const float &_u, const float &_v, const float &_w) const {
	return _u * u + _v * v + _w * w;
}

glm::vec3 ONB::localFromW(const glm::vec3 &_w, const glm::vec3 &uvw) {
	glm::vec3 w = glm::normalize(_w);
	glm::vec3 a;
	if (std::fabs(w.x) > 0.9)
		a = glm::vec3(0, 1, 0);
	else
		a = glm::vec3(1, 0, 0);
	glm::vec3 v = glm::normalize(glm::cross(w, a));
	glm::vec3 u = glm::normalize(glm::cross(w, v));
	return u * uvw.x + v * uvw.y + w * uvw.z;
}
