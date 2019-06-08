//
// Created by 何振邦 on 2019-05-30.
//
#define STB_IMAGE_IMPLEMENTATION

#include "texture.h"

SolidColorTexture::SolidColorTexture(glm::vec3 _color) : color(_color) {

}

glm::vec3 SolidColorTexture::getColor(const glm::vec2 &uv) const {
	return color;
}

ImageTexture::ImageTexture(std::string path) {
	loadFromPath(path);
}

ImageTexture::ImageTexture(const ImageTexture &imgTex) {
	width = imgTex.width;
	height = imgTex.height;
	channel = imgTex.channel;
	if (data != nullptr)stbi_image_free(data);
	data = new unsigned char[width * height * channel];
	std::memcpy(data, imgTex.data, width * height * channel);
}

ImageTexture &ImageTexture::operator=(const ImageTexture &imgTex) {
	width = imgTex.width;
	height = imgTex.height;
	channel = imgTex.channel;
	if (data != nullptr)stbi_image_free(data);
	data = new unsigned char[width * height * channel];
	std::memcpy(data, imgTex.data, width * height * channel);
	return *this;
}

bool ImageTexture::loadFromPath(std::string path) {
	auto tempdata = stbi_load(path.c_str(), &width, &height, &channel, 0);
	if (tempdata == nullptr) {
		std::cout << "unable to load texture data from image:" << path << std::endl;
		return false;
	}
	if (data != nullptr)stbi_image_free(data);
	data = tempdata;
	return true;
}

glm::vec3 ImageTexture::getColor(const glm::vec2 &uv) const {
	if (data == nullptr)return glm::vec3(0);
	float u, v;
	//TODO. extend filter method
	u = uv.x < 0 ? 0 : uv.x > 1 ? 1 : uv.x;
	v = uv.y < 0 ? 0 : uv.y > 1 ? 1 : uv.y;
	int x = u * (width - 1);
	int y = v * (height - 1);
	return glm::vec3(static_cast<int>(data[(height - 1 - y) * width * channel + x * channel]) / 255.0,
	            static_cast<int>(data[(height - 1 - y) * width * channel + x * channel + 1]) / 255.0,
	            static_cast<int>(data[(height - 1 - y) * width * channel + x * channel + 2]) / 255.0);
}