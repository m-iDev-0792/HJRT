//
// Created by 何振邦 on 2019-05-30.
//
#define STB_IMAGE_IMPLEMENTATION

#include "texture.h"

//----------------------------------
//         SamplingTexture
//----------------------------------

bool SamplingTexture::setSamplesFromImage(std::string _path) {
	int channel;
	auto tempdata = stbi_load(_path.c_str(), &width, &height, &channel, 0);
	if (tempdata == nullptr) {
		Error("unable to load texture data from image:" + _path + "\n");
		return false;
	}
	if (channel > 1)Warning("texture has more than 1 channel, use sum of all channals as sampling number\n");

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

int SamplingTexture::getColor(const glm::vec2 &_uv) const {
	if (data == nullptr)
		return uniformSamplingNum;
	int u = _uv.x < 0 ? 0 : _uv.x >= width ? width - 1 : _uv.x;
	int v = _uv.y < 0 ? 0 : _uv.y >= height ? height - 1 : _uv.y;
	return data[(height - 1 - v) * width + u];
}

std::shared_ptr<ImageTexture<glm::vec3>> makeRGBImageTexture(std::string path) {
	int width, height, channel;
	auto tempdata = stbi_load(path.c_str(), &width, &height, &channel, 0);
	if (tempdata == nullptr) {
		Error("unable to load texture data from image:" + path + "\n");
		return nullptr;
	}
	if (channel == 1) {
		Note("image only has 1 channel, use 1 channel to fill 3 channels\n");
	} else if (channel == 2) {
		Error("image has only 2 channels, can't be loaded as RGB format\n");
		return nullptr;
	} else if (channel > 3) {
		Warning("loaded image has more than 3 channels, but only 3 channels data will be loaded\n");
	}
	glm::vec3 *data = new glm::vec3[width * height];
	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {
			if (channel == 1) {
				data[(height - 1 - j) * width + i] = glm::vec3(tempdata[(height - 1 - j) * width + i] / 255.0);
			} else if (channel >= 3) {
				data[(height - 1 - j) * width + i] = glm::vec3(
						tempdata[(height - 1 - j) * width * channel + i * channel] / 255.0,
						tempdata[(height - 1 - j) * width * channel + i * channel + 1] / 255.0,
						tempdata[(height - 1 - j) * width * channel + i * channel + 2] / 255.0);
			}
		}
	}
	return std::make_shared<ImageTexture<glm::vec3>>(data, width, height);
}

std::shared_ptr<ImageTexture<glm::vec4>> makeRGBAImageTexture(std::string path) {
	int width, height, channel;
	auto tempdata = stbi_load(path.c_str(), &width, &height, &channel, 0);
	if (tempdata == nullptr) {
		Error("unable to load texture data from image:" + path + "\n");
		return nullptr;
	}
	if (channel < 4) {
		Error("image has less than 4 channels\n");
		return nullptr;
	}
	glm::vec4 *data = new glm::vec4[width * height];
	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {
			if (channel >= 4)
				data[(height - 1 - j) * width + i] = glm::vec4(
						tempdata[(height - 1 - j) * width * channel + i * channel] / 255.0,
						tempdata[(height - 1 - j) * width * channel + i * channel + 1] / 255.0,
						tempdata[(height - 1 - j) * width * channel + i * channel + 2] / 255.0,
						tempdata[(height - 1 - j) * width * channel + i * channel + 3] / 255.0);
		}
	}
	return std::make_shared<ImageTexture<glm::vec4>>(data, width, height);
}

std::shared_ptr<ImageTexture<float>> makeGrayImageTexture(std::string path) {
	int width, height, channel;
	auto tempdata = stbi_load(path.c_str(), &width, &height, &channel, 0);
	if (tempdata == nullptr) {
		Error("unable to load texture data from image:" + path + "\n");
		return nullptr;
	}
	if (channel > 1) {
		Note("image has more than 1 channel, the other channels will be discarded\n");
	}
	float *data = new float[width * height];
	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {
			if (channel == 1) {
				data[(height - 1 - j) * width + i] = tempdata[(height - 1 - j) * width + i] / 255.0;
			} else if (channel > 1) {
				data[(height - 1 - j) * width + i] = tempdata[(height - 1 - j) * width * channel + i * channel] / 255.0;
			}
		}
	}
	return std::make_shared<ImageTexture<float>>(data, width, height);
}