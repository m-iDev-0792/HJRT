//
// Created by 何振邦 on 2019-05-15.
//

#include "Camera.h"
Camera::Camera(glm::vec3 _position,glm::vec3 _direction,int _width,int _height,float _zNear,float _fov):position(_position),direction(_direction),width(_width),height(_height),zNear(_zNear),fov(_fov){
	if(glm::length(direction-vec3(0,1,0))>0.01f)
		right=-normalize(cross(vec3(0,1,0),direction));
	else
		right=vec3(1,0,0);//TODO. potential bug!!!!
	up=normalize(cross(right,direction));

	actualHeight=zNear*tan(radians(fov/2))*2;
	actualWidth=actualHeight*width/height;
	pixelWidth=actualWidth/width;
	pixelHeight=actualHeight/height;
}
Ray Camera::getRay(float u,float v){
	vec3 pixel=position+direction*zNear+right*(u-width/2+0.5f)*pixelWidth+up*(v-height/2+0.5f)*pixelHeight;
	return Ray(pixel,normalize(pixel-position));
}