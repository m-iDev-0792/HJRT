//
// Created by 何振邦 on 2019-05-15.
//

#ifndef RTTEST_CAMERA_H
#define RTTEST_CAMERA_H
#include "utility.h"
#include "ray.h"
struct Camera {
	glm::vec3 position;
	glm::vec3 direction;
	const float zNear=1.0;//TODO. remove zNear and zFar?
	float zFar;
	float fov;
	int width;
	int height;
	float aperture;
	float zFocus;
	Camera()= default;
	Camera(glm::vec3 _position,glm::vec3 _direction,int _width,int _height,float _fov,float _aperture=-1,float _zFocus=1);
	Ray getRay(const float& u,const float& v);
private:
	glm::vec3 right;
	glm::vec3 up;
	glm::vec3 lowerLeftPixel;

	float actualHeight;
	float actualWidth;
	float pixelWidth;
	float pixelHeight;
};


#endif //RTTEST_CAMERA_H
