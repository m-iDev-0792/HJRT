//
// Created by 何振邦 on 2019-05-30.
//
#define STB_IMAGE_IMPLEMENTATION

#include "texture.h"

SolidColorTexture::SolidColorTexture(vec3 _color) : color(_color) {

}

vec3 SolidColorTexture::getColor(const vec2 &uv) const {
	return color;
}

ImageTexture::ImageTexture(string path) {
	loadFromPath(path);
}

ImageTexture::ImageTexture(const ImageTexture &imgTex) {
	width = imgTex.width;
	height = imgTex.height;
	channel = imgTex.channel;
	if (data != nullptr)stbi_image_free(data);
	data = new unsigned char[width * height * channel];
	memcpy(data, imgTex.data, width * height * channel);
}

ImageTexture &ImageTexture::operator=(const ImageTexture &imgTex) {
	width = imgTex.width;
	height = imgTex.height;
	channel = imgTex.channel;
	if (data != nullptr)stbi_image_free(data);
	data = new unsigned char[width * height * channel];
	memcpy(data, imgTex.data, width * height * channel);
	return *this;
}

bool ImageTexture::loadFromPath(string path) {
	auto tempdata = stbi_load(path.c_str(), &width, &height, &channel, 0);
	if (tempdata == nullptr) {
		cout << "unable to load texture data from image:" << path << endl;
		return false;
	}
	if (data != nullptr)stbi_image_free(data);
	data = tempdata;
	return true;
}

vec3 ImageTexture::getColor(const vec2 &uv) const {
	if (data == nullptr)return vec3(0);
	float u, v;
	//TODO. extend filter method
	u = uv.x < 0 ? 0 : uv.x > 1 ? 1 : uv.x;
	v = uv.y < 0 ? 0 : uv.y > 1 ? 1 : uv.y;
	int x = u * (width - 1);
	int y = v * (height - 1);
	return vec3(static_cast<int>(data[(height - 1 - y) * width * channel + x * channel]) / 255.0,
	            static_cast<int>(data[(height - 1 - y) * width * channel + x * channel + 1]) / 255.0,
	            static_cast<int>(data[(height - 1 - y) * width * channel + x * channel + 2]) / 255.0);
}