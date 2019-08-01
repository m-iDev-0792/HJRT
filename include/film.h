//
// Created by 何振邦 on 2019-05-21.
//

#ifndef RTTEST_IMAGEEXPORTER_H
#define RTTEST_IMAGEEXPORTER_H

#include "glm/glm.hpp"
#include "stb/stb_image_write.h"
#include "tinyexr/tinyexr.h"
#include "utility.h"
#include <fstream>

struct Film {
	int width;
	int height;
	unsigned char *data;
	glm::vec3 *rawData;

	Film(int _width, int _height);

	~Film() {
		delete[] data;
		delete[] rawData;
	}

	const unsigned char *getData() { return data; }

	const glm::vec3 *getRawData() { return rawData; }

	void setPixel(int row, int column, const glm::vec3 &color);

	bool save(std::string fileName, std::string format) const;
};


#endif //RTTEST_IMAGEEXPORTER_H
