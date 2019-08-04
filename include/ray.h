//
// Created by 何振邦 on 2019-07-19.
//

#ifndef HJRT_RAY_H
#define HJRT_RAY_H

#include "glm/glm.hpp"

constexpr float RAY_T_INFINITY = 10e10;
constexpr float RAY_T_DEFAULT_MIN = 10e-4;
struct Shape;

struct TimePeriod {
	float start;
	float end;

	TimePeriod() = default;

	TimePeriod(float _start, float _end) : start(_start), end(_end) {}
};

struct HitInfo {
	float t;
	float hittime;
	glm::vec3 hitpoint;
	glm::vec3 normal;//WARNING!!! keep normal normalized!!!
	glm::vec2 uv;
	const Shape *hitobject;

	HitInfo() {
		t = RAY_T_INFINITY;
		hitobject = nullptr;
	}
};

struct Ray {
	glm::vec3 origin;
	glm::vec3 dir;
	float time;
	float tMax;
	float tMin;

	Ray() {
		time = 0;
		tMin = RAY_T_DEFAULT_MIN;
		tMax = RAY_T_INFINITY;
	}
	//NOTE!! in Camera::castRay dosen's use tMin and tMax parameters
	Ray(glm::vec3 _origin, glm::vec3 _dir, float _time = 0, float _tMin = RAY_T_DEFAULT_MIN,
	    float _tMax = RAY_T_INFINITY) : origin(_origin), dir(_dir),
	                                    time(_time), tMin(_tMin),
	                                    tMax(_tMax) {}
};


#endif //HJRT_RAY_H
