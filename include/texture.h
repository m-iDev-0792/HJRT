//
// Created by 何振邦 on 2019-05-30.
//

#ifndef RTTEST_TEXTURE_H
#define RTTEST_TEXTURE_H

#include "utility.h"
#include <stb/stb_image.h>
#include <memory>
struct Texture {
	virtual glm::vec3 getColor(const glm::vec2& uv) const = 0;
};

struct SolidColorTexture : Texture {
	glm::vec3 color;

	SolidColorTexture() = default;

	SolidColorTexture(glm::vec3 _color);

	glm::vec3 getColor(const glm::vec2& uv) const override;
};

struct ImageTexture : Texture {
	unsigned char* data;
	int width;
	int height;
	int channel;

	ImageTexture() = default;

	ImageTexture(std::string path);

	ImageTexture(const ImageTexture& imgTex);

	ImageTexture& operator = (const ImageTexture& imgTex);

	bool loadFromPath(std::string path);

	glm::vec3 getColor(const glm::vec2& uv) const override;
};

#endif //RTTEST_TEXTURE_H
