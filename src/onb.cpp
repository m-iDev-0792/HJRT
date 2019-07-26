//
// Created by 何振邦 on 2019-07-19.
//
#include "onb.h"

ONB::ONB(ONB &_another) {
	for (int i = 0; i < 3; ++i)
		(*this)[i] = _another[i];
}

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

glm::vec3 ONB::local(const glm::vec3 &_pos) const {
	return u * _pos.x + v * _pos.y + w * _pos.z;
}

glm::vec3 ONB::local(const float &_x, const float &_y, const float &_z) const {
	return _x * u + _y * v + _z * w;
}

glm::vec3 ONB::localFromW(const glm::vec3 &_w, const glm::vec3 &_pos) {
	glm::vec3 w = glm::normalize(_w);
	glm::vec3 a;
	if (std::fabs(w.x) > 0.9)
		a = glm::vec3(0, 1, 0);
	else
		a = glm::vec3(1, 0, 0);
	glm::vec3 v = glm::normalize(glm::cross(w, a));
	glm::vec3 u = glm::normalize(glm::cross(w, v));
	return u * _pos.x + v * _pos.y + w * _pos.z;
}
