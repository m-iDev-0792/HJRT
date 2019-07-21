//
// Created by 何振邦 on 2019-07-19.
//
//Ortho Normal Base
#ifndef HJRT_ONB_H
#define HJRT_ONB_H

#include "utility.h"
struct ONB{
	struct{
		glm::vec3 u;
		glm::vec3 v;
		glm::vec3 w;
	};
	ONB(glm::vec3 _w);
	ONB(glm::vec3 _u,glm::vec3 _v,glm::vec3 _w);
	glm::vec3& operator [](int index){
		index=index>2?2:index<0?0:index;
		return (&u)[index];
	}
	glm::vec3 local(const glm::vec3& uvw)const;
	glm::vec3 local(const float& _u,const float& _v,const float& _w)const;
	static glm::vec3 localFromW(const glm::vec3& _w, const glm::vec3& uvw);
};


#endif //HJRT_ONB_H
