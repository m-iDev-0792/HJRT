//
// Created by 何振邦 on 2019-05-15.
//

#ifndef RTTEST_CAMERA_H
#define RTTEST_CAMERA_H
#include "utility.h"

struct Camera {
	glm::vec3 position;
	glm::vec3 direction;
	float zNear;
	float zFar;
	float fov;
	int width;
	int height;
	Camera()= default;
	Camera(glm::vec3 _position,glm::vec3 _direction,int _width,int _height,float _zNear,float _fov);
	Ray getRay(float u,float v);
private:
	glm::vec3 right;
	glm::vec3 up;

	float actualHeight;
	float actualWidth;
	float pixelWidth;
	float pixelHeight;
};


#endif //RTTEST_CAMERA_H
