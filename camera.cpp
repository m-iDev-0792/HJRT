//
// Created by 何振邦 on 2019-05-15.
//

#include "camera.h"
Camera::Camera(glm::vec3 _position,glm::vec3 _direction,int _width,int _height,float _fov,float _aperture,float _zFocus)
:position(_position),direction(_direction),width(_width),height(_height),fov(_fov),aperture(_aperture),zFocus(_zFocus){
	if(glm::length(direction-vec3(0,1,0))>0.01f)
		right=-normalize(cross(vec3(0,1,0),direction));
	else
		right=vec3(1,0,0);//TODO. potential bug!!!!
	up=normalize(cross(right,direction));

	actualHeight=zNear*tan(radians(fov/2))*2;
	actualWidth=actualHeight*width/height;
	pixelWidth=actualWidth/width;
	pixelHeight=actualHeight/height;

	lowerLeftPixel=position + direction*zNear*zFocus + right*pixelWidth*zFocus*(-width/2+0.5f) + up*pixelHeight*zFocus*(-height/2+0.5f);
}
Ray Camera::getRay(const float& u,const float& v){
	if(aperture>0){
		vec3 rd=aperture*0.5f* randomVecUnitDisk();
		vec3 offset=right*rd.x+up*rd.y;
		vec3 pixel=lowerLeftPixel + right*pixelWidth*zFocus*u + up*pixelHeight*zFocus*v;
		return Ray(position+offset,normalize(pixel-position-offset));
	} else{
		vec3 pixel=lowerLeftPixel + right*pixelWidth*zFocus*u + up*pixelHeight*zFocus*v;
		return Ray(position,normalize(pixel-position));
	}

}