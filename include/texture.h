//
// Created by 何振邦 on 2019-05-30.
//

#ifndef RTTEST_TEXTURE_H
#define RTTEST_TEXTURE_H

#include "utility.h"
#include <stb/stb_image.h>
#include <memory>

template<typename T>
struct Texture {
	virtual T getColor(const glm::vec2 &_uv) const = 0;
};

template<typename T>
struct SolidColorTexture : Texture<T> {
	T color;

	SolidColorTexture() = default;

	SolidColorTexture(T _color):color(_color){}

	T getColor(const glm::vec2 &_uv) const override{return color;}
};

template<typename T>
struct ImageTexture : Texture<T> {
	T *data;
	int width;
	int height;

	ImageTexture() { data = nullptr; };

	ImageTexture(T *_data, int _width, int _height) {
		getData(_data, _width, _height);
	}

	ImageTexture(const ImageTexture<T> &_imgTex) {
		width = _imgTex.width;
		height = _imgTex.height;
		data = new T[width * height];
		std::memcpy(data, _imgTex.data, width * height * sizeof(T));
	}

	ImageTexture &operator=(const ImageTexture<T> &_imgTex) {
		if (&_imgTex == this)return *this;//avoid self assignment
		width = _imgTex.width;
		height = _imgTex.height;
		if (data != nullptr)delete[] data;
		data = new T[width * height];
		std::memcpy(data, _imgTex.data, width * height * sizeof(T));
		return *this;
	}

	void getData(T *_data, int _width, int _height) {
		width = _width;
		height = _height;
		data = _data;
	}

	void copyData(T *_data, int _width, int _height) {
		width = _width;
		height = _height;
		data = new T[width * height];
		std::memcpy(data, _data, width * height * sizeof(T));
	}

	T getColor(const glm::vec2 &_uv) const override{
		if (data == nullptr)return T(0);
		float u, v;
		u = _uv.x < 0 ? _uv.x - static_cast<int>(_uv.x) + 1 : _uv.x > 1 ? _uv.x - static_cast<int>(_uv.x) : _uv.x;
		v = _uv.y < 0 ? _uv.y - static_cast<int>(_uv.y) + 1 : _uv.y > 1 ? _uv.y - static_cast<int>(_uv.y) : _uv.y;
		int x = u * (width - 1);
		int y = v * (height - 1);
		return data[(height - 1 - y) * width + x];
	}
};

struct SamplingTexture : public ImageTexture<int> {
private:

	int uniformSamplingNum;
public:
	SamplingTexture() {
		data = nullptr;
		uniformSamplingNum = 1;
	}

	SamplingTexture(int _samples) : uniformSamplingNum(_samples) {
		data= nullptr;
	}

	SamplingTexture(std::string _path){
		data = nullptr;
		setSamplesFromImage(_path);
	}

	int getColor(const glm::vec2 &_uv) const override;

	void setSamples(int _samples) {
		if (data != nullptr)delete[] data;
		data = nullptr;
		uniformSamplingNum = _samples;
	}

	bool setSamplesFromImage(std::string _path);

	int getUniformSamples() const{
		return data == nullptr ? uniformSamplingNum : -1;
	}
};

std::shared_ptr<ImageTexture<glm::vec3>> makeRGBImageTexture(std::string path);

std::shared_ptr<ImageTexture<float>> makeGrayImageTexture(std::string path);

#endif //RTTEST_TEXTURE_H
