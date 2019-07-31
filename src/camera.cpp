//
// Created by 何振邦 on 2019-05-15.
//

#include "camera.h"

Camera::Camera(glm::vec3 _position, glm::vec3 _direction, int _width, int _height, float _fov, float _aperture,
               float _zFocus)
		: position(_position), direction(_direction), width(_width), height(_height), fov(_fov), aperture(_aperture),
		  zFocus(_zFocus) {
	if (glm::length(direction - glm::vec3(0, 1, 0)) > 0.01f)
		right = -glm::normalize(glm::cross(glm::vec3(0, 1, 0), direction));
	else
		right = glm::vec3(1, 0, 0);//TODO. potential bug!!!!
	up = glm::normalize(glm::cross(right, direction));

	actualHeight = zNear * std::tan(glm::radians(fov / 2)) * 2;
	actualWidth = actualHeight * width / height;
	pixelWidth = actualWidth / width;
	pixelHeight = actualHeight / height;

	lowerLeftPixel = position + direction * zNear * zFocus + right * pixelWidth * zFocus * (-width / 2 + 0.5f) +
	                 up * pixelHeight * zFocus * (-height / 2 + 0.5f);
}

Ray Camera::castRay(const float &u, const float &v) {
	if (aperture > 0) {
		glm::vec3 rd = aperture * 0.5f * sampleInsideDiskUniform();
		glm::vec3 offset = right * rd.x + up * rd.y;
		glm::vec3 pixel = lowerLeftPixel + right * pixelWidth * zFocus * u + up * pixelHeight * zFocus * v;
		return Ray(position + offset, glm::normalize(pixel - position - offset));
	} else {
		glm::vec3 pixel = lowerLeftPixel + right * pixelWidth * zFocus * u + up * pixelHeight * zFocus * v;
		return Ray(position, glm::normalize(pixel - position));
	}

}

Ray Camera::castRay(const float &u, const float &v, const TimePeriod &period) {
	Ray ray = castRay(u, v);
	ray.time = period.start + random0_1f() * (period.end - period.start);
	return ray;
}