//
// Created by 何振邦 on 2019-05-30.
//

#ifndef RTTEST_TEXTURE_H
#define RTTEST_TEXTURE_H

#include "utility.h"
#include <stb/stb_image.h>
#include <memory>

struct Texture {
	virtual glm::vec3 getColor(const glm::vec2 &_uv) const = 0;
};

struct SolidColorTexture : Texture {
	glm::vec3 color;

	SolidColorTexture() = default;

	SolidColorTexture(glm::vec3 _color);

	SolidColorTexture(float _r, float _g, float _b);

	glm::vec3 getColor(const glm::vec2 &_uv) const override;
};

struct ImageTexture : Texture {
	unsigned char *data;
	int width;
	int height;
	int channel;

	ImageTexture() { data = nullptr; };

	ImageTexture(std::string _path);

	ImageTexture(const ImageTexture &_imgTex);

	ImageTexture &operator=(const ImageTexture &_imgTex);

	bool loadFromPath(std::string _path);

	glm::vec3 getColor(const glm::vec2 &_uv) const override;
};

struct SamplingTexture : public Texture {
private:
	int width;
	int height;
	int *data;
	int uniformSamplingNum;
public:
	SamplingTexture() {
		data = nullptr;
		uniformSamplingNum = 1;
	}

	SamplingTexture(int _samples) : uniformSamplingNum(_samples) {}

	SamplingTexture(std::string _path);

	SamplingTexture(const SamplingTexture &_samplingTex);

	SamplingTexture &operator=(const SamplingTexture &_samplingTex);

	bool loadFromImage(std::string _path);

	glm::vec3 getColor(const glm::vec2 &_uv) const override;

	void setSamples(int _samples) {
		if (data != nullptr)delete[] data;
		data = nullptr;
		uniformSamplingNum = _samples;
	}

	int getUniformSamples() {
		return data == nullptr ? uniformSamplingNum : -1;
	}
};

#endif //RTTEST_TEXTURE_H
