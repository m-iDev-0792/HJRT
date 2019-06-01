//
// Created by 何振邦 on 2019-05-21.
//

#ifndef RTTEST_IMAGEEXPORTER_H
#define RTTEST_IMAGEEXPORTER_H
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <stb/stb_image_write.h>
using namespace glm;
using namespace std;
struct ImageExporter {
	int width;
	int height;
	unsigned char* data;

	ImageExporter(int _width,int _height);
	~ImageExporter(){delete [] data;}
	void setPixel(int row,int column,const vec3& color);
	bool save(string fileName,string format)const;
};


#endif //RTTEST_IMAGEEXPORTER_H
