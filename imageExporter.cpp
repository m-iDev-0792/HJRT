//
// Created by 何振邦 on 2019-05-21.
//
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "imageExporter.h"
ImageExporter::ImageExporter(int _width, int _height):width(_width),height(_height){
	data=new unsigned char[width*height*3];
}
void ImageExporter::setPixel(int row, int column, const vec3 &color) {
	data[((height-1-row)*width+column)*3]=color.x;
	data[((height-1-row)*width+column)*3+1]=color.y;
	data[((height-1-row)*width+column)*3+2]=color.z;
}
bool ImageExporter::save(string fileName, string format)const {
	if(format=="png"){
		return stbi_write_png(fileName.c_str(),width,height,3,data, 0);
	}else if(format=="jpg"){
		return stbi_write_jpg(fileName.c_str(),width,height,3,data,0);
	}else if(format=="bmp"){
		return stbi_write_bmp(fileName.c_str(),width,height,3,data);
	}else if(format=="tga"){
		return stbi_write_tga(fileName.c_str(),width,height,3,data);
	}else if(format=="ppm"){
		ofstream file(fileName);
		if(!file.is_open())return false;
		file<<"P3\n";
		file<<width<<" "<<height<<"\n";
		file<<"255\n";
		for(int i=0;i<height;++i){
			for(int j=0;j<width;++j){
				unsigned char* color=&data[(i*width+j)*3];
				file<< static_cast<int>(color[0])<<" "<<static_cast<int>(color[1])<<" "<<static_cast<int>(color[2])<<" ";
			}
			file<<"\n";
		}
		return true;
	}
	else return false;
}