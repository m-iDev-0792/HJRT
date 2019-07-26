//
// Created by 何振邦 on 2019-07-19.
//
//Ortho Normal Base
#ifndef HJRT_ONB_H
#define HJRT_ONB_H

#include "utility.h"

struct ONB {
	struct {
		glm::vec3 u;
		glm::vec3 v;
		glm::vec3 w;
	};

	ONB(ONB &_another);

	ONB(glm::vec3 _w);

	ONB(glm::vec3 _u, glm::vec3 _v, glm::vec3 _w);

	glm::vec3 &operator[](int index) {
		index = index > 2 ? 2 : index < 0 ? 0 : index;
		return (&u)[index];
	}

	glm::vec3 local(const glm::vec3 &_pos) const;

	glm::vec3 local(const float &_x, const float &_y, const float &_z) const;

	static glm::vec3 localFromW(const glm::vec3 &_w, const glm::vec3 &_pos);
};


#endif //HJRT_ONB_H
