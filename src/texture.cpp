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
		std::cout << "ERROR @ ImageTexture : unable to load texture data from image:" << _path << std::endl;
		return false;
	}
	if (data != nullptr)stbi_image_free(data);
	data = tempdata;
	return true;
}

glm::vec3 ImageTexture::getColor(const glm::vec2 &_uv) const {
	if (data == nullptr)return glm::vec3(0);
	float u, v;
	u = _uv.x < 0 ? _uv.x - static_cast<int>(_uv.x) + 1 : _uv.x > 1 ? _uv.x - static_cast<int>(_uv.x) : _uv.x;
	v = _uv.y < 0 ? _uv.y - static_cast<int>(_uv.y) + 1 : _uv.y > 1 ? _uv.y - static_cast<int>(_uv.y) : _uv.y;
	int x = u * (width - 1);
	int y = v * (height - 1);
	if (channel == 1)
		return glm::vec3(static_cast<int>(data[(height - 1 - y) * width + x]) / 255.0);
	else
		return glm::vec3(static_cast<int>(data[(height - 1 - y) * width * channel + x * channel]) / 255.0,
		                 static_cast<int>(data[(height - 1 - y) * width * channel + x * channel + 1]) / 255.0,
		                 static_cast<int>(data[(height - 1 - y) * width * channel + x * channel + 2]) / 255.0);
}

SamplingTexture::SamplingTexture(std::string _path) {
	data = nullptr;
	loadFromImage(_path);
}

SamplingTexture::SamplingTexture(const SamplingTexture &_samplingTex) {
	width = _samplingTex.width;
	height = _samplingTex.height;
	data = new int[width * height];
	std::memcpy(data, _samplingTex.data, width * height);
}

SamplingTexture &SamplingTexture::operator=(const SamplingTexture &_samplingTex) {
	if (this == &_samplingTex)return *this;
	width = _samplingTex.width;
	height = _samplingTex.height;
	if (data != nullptr)delete[] data;
	data = new int[width * height];
	std::memcpy(data, _samplingTex.data, width * height);
}

bool SamplingTexture::loadFromImage(std::string _path) {
	int channel;
	auto tempdata = stbi_load(_path.c_str(), &width, &height, &channel, 0);
	if (tempdata == nullptr) {
		std::cout << "ERROR @ ImageTexture : unable to load texture data from image:" << _path << std::endl;
		return false;
	}
	if (data != nullptr)delete[] data;
	data = new int[width * height];
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			if (channel == 1) {
				data[(height - 1 - j) * width + i] = tempdata[(height - 1 - j) * width + i];
			} else if (channel == 3) {
				data[(height - 1 - j) * width + i] = tempdata[(height - 1 - j) * width * channel + i * channel] +
				                                     tempdata[(height - 1 - j) * width * channel + i * channel + 1] +
				                                     tempdata[(height - 1 - j) * width * channel + i * channel + 2];
			}
		}
	}
	stbi_image_free(tempdata);
	return true;
}

glm::vec3 SamplingTexture::getColor(const glm::vec2 &_uv) const {
	if (data == nullptr)
		return glm::vec3(uniformSamplingNum);
	int u = _uv.x < 0 ? 0 : _uv.x >= width ? width - 1 : _uv.x;
	int v = _uv.y < 0 ? 0 : _uv.y >= height ? height - 1 : _uv.y;
	return glm::vec3(data[(height - 1 - v) * width + u]);

}