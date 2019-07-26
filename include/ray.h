//
// Created by 何振邦 on 2019-07-19.
//

#ifndef HJRT_RAY_H
#define HJRT_RAY_H
#include <glm/glm.hpp>
struct Object;
struct HitInfo {
	float t;
	glm::vec3 hitpoint;
	glm::vec3 normal;//WARNING!!! keep normal normalized!!!
	glm::vec2 uv;
	const Object *hitobject;
	HitInfo(){t=10e10;hitobject= nullptr;}
};

struct Ray {
	glm::vec3 origin;
	glm::vec3 dir;

	Ray() = default;

	Ray(glm::vec3 _origin, glm::vec3 _dir) : origin(_origin), dir(_dir) {}
};


#endif //HJRT_RAY_H
