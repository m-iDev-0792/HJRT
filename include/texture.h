//
// Created by 何振邦 on 2019-05-30.
//

#ifndef RTTEST_TEXTURE_H
#define RTTEST_TEXTURE_H

#include "utility.h"
#include <stb/stb_image.h>
#include <memory>
struct Texture {
	virtual vec3 getColor(const vec2& uv) const = 0;
};

struct SolidColorTexture : Texture {
	vec3 color;

	SolidColorTexture() = default;

	SolidColorTexture(vec3 _color);

	vec3 getColor(const vec2& uv) const override;
};

struct ImageTexture : Texture {
	unsigned char* data;
	int width;
	int height;
	int channel;

	ImageTexture() = default;

	ImageTexture(string path);

	ImageTexture(const ImageTexture& imgTex);

	ImageTexture& operator = (const ImageTexture& imgTex);

	bool loadFromPath(string path);

	vec3 getColor(const vec2& uv) const override;
};

#endif //RTTEST_TEXTURE_H
