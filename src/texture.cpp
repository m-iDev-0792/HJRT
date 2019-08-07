//
// Created by 何振邦 on 2019-05-30.
//
#define STB_IMAGE_IMPLEMENTATION

#include "texture.h"

SolidColorTexture::SolidColorTexture(glm::vec3 _color) : color(_color) {

}

SolidColorTexture::SolidColorTexture(float _r, float _g, float _b) {
	color = glm::vec3(_r, _g, _b);
}

glm::vec3 SolidColorTexture::getColor(const glm::vec2 &_uv) const {
	return color;
}

ImageTexture::ImageTexture(std::string _path) {
	data = nullptr;
	loadFromPath(_path);
}

ImageTexture::ImageTexture(const ImageTexture &_imgTex) {
	width = _imgTex.width;
	height = _imgTex.height;
	channel = _imgTex.channel;
	data = new unsigned char[width * height * channel];
	std::memcpy(data, _imgTex.data, width * height * channel);
}

ImageTexture &ImageTexture::operator=(const ImageTexture &_imgTex) {
	if (&_imgTex == this)return *this;//avoid self assignment
	width = _imgTex.width;
	height = _imgTex.height;
	channel = _imgTex.channel;
	if (data != nullptr)stbi_image_free(data);
	data = new unsigned char[width * height * channel];
	std::memcpy(data, _imgTex.data, width * height * channel);
	return *this;
}

bool ImageTexture::loadFromPath(std::string _path) {
	auto tempdata = stbi_load(_path.c_str(), &width, &height, &channel, 0);
	if (tempdata == nullptr) {
		std::cout << "unable to load texture data from image:" << _path << std::endl;
		return false;
	}
	if (data != nullptr)stbi_image_free(data);
	data = tempdata;
	return true;
}

glm::vec3 ImageTexture::getColor(const glm::vec2 &_uv) const {
	if (data == nullptr)return glm::vec3(0);
	float u, v;
	//TODO. extend filter method
	u = _uv.x < 0 ? 0 : _uv.x > 1 ? 1 : _uv.x;
	v = _uv.y < 0 ? 0 : _uv.y > 1 ? 1 : _uv.y;
	int x = u * (width - 1);
	int y = v * (height - 1);
	if (channel == 1)
		return glm::vec3(static_cast<int>(data[(height - 1 - y) * width + x]) / 255.0);
	else
		return glm::vec3(static_cast<int>(data[(height - 1 - y) * width * channel + x * channel]) / 255.0,
		                 static_cast<int>(data[(height - 1 - y) * width * channel + x * channel + 1]) / 255.0,
		                 static_cast<int>(data[(height - 1 - y) * width * channel + x * channel + 2]) / 255.0);
}