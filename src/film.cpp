//
// Created by 何振邦 on 2019-05-21.
//
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYEXR_IMPLEMENTATION

#include "film.h"

inline void toInt(glm::vec3 &color) {
	color *= 255;
	color.r = (color.r > 255 ? 255 : (color.r < 0 ? 0 : static_cast<int>(color.r)));
	color.g = (color.g > 255 ? 255 : (color.g < 0 ? 0 : static_cast<int>(color.g)));
	color.b = (color.b > 255 ? 255 : (color.b < 0 ? 0 : static_cast<int>(color.b)));
}

Film::Film(int _width, int _height) : width(_width), height(_height) {
	discretizedData = new unsigned char[width * height * 3];
	rawData = new glm::vec3[width * height * 3];
}

void Film::setPixel(int row, int column, const glm::vec3 &color) {
	glm::vec3 c = color;
	rawData[((height - 1 - row) * width + column)] = c;
	gammaCorrection(c);
	toInt(c);
	discretizedData[((height - 1 - row) * width + column) * 3] = c.x;
	discretizedData[((height - 1 - row) * width + column) * 3 + 1] = c.y;
	discretizedData[((height - 1 - row) * width + column) * 3 + 2] = c.z;
}

bool SaveEXR(const float *rgb, int width, int height, const char *outfilename);

bool Film::save(std::string fileName) const {
	std::string format = getFileType(fileName);
	if (format == "png") {
		return stbi_write_png(fileName.c_str(), width, height, 3, discretizedData, 0);
	} else if (format == "jpg") {
		return stbi_write_jpg(fileName.c_str(), width, height, 3, discretizedData, 0);
	} else if (format == "bmp") {
		return stbi_write_bmp(fileName.c_str(), width, height, 3, discretizedData);
	} else if (format == "tga") {
		return stbi_write_tga(fileName.c_str(), width, height, 3, discretizedData);
	} else if (format == "ppm") {
		std::ofstream file(fileName);
		if (!file.is_open())return false;
		file << "P3\n";
		file << width << " " << height << "\n";
		file << "255\n";
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				unsigned char *color = &discretizedData[(i * width + j) * 3];
				file << static_cast<int>(color[0]) << " " << static_cast<int>(color[1]) << " "
				     << static_cast<int>(color[2]) << " ";
			}
			file << "\n";
		}
		return true;
	} else if (format == "exr") {
		return SaveEXR((float *) rawData, width, height, fileName.c_str());
	} else return false;
}

bool SaveEXR(const float *rgb, int width, int height, const char *outfilename) {

	EXRHeader header;
	InitEXRHeader(&header);

	EXRImage image;
	InitEXRImage(&image);

	image.num_channels = 3;

	std::vector<float> images[3];
	images[0].resize(width * height);
	images[1].resize(width * height);
	images[2].resize(width * height);

	// Split RGBRGBRGB... into R, G and B layer
	for (int i = 0; i < width * height; i++) {
		images[0][i] = rgb[3 * i + 0];
		images[1][i] = rgb[3 * i + 1];
		images[2][i] = rgb[3 * i + 2];
	}

	float *image_ptr[3];
	image_ptr[0] = &(images[2].at(0)); // B
	image_ptr[1] = &(images[1].at(0)); // G
	image_ptr[2] = &(images[0].at(0)); // R

	image.images = (unsigned char **) image_ptr;
	image.width = width;
	image.height = height;

	header.num_channels = 3;
	header.channels = (EXRChannelInfo *) malloc(sizeof(EXRChannelInfo) * header.num_channels);
	// Must be (A)BGR order, since most of EXR viewers expect this channel order.
	strncpy(header.channels[0].name, "B", 255);
	header.channels[0].name[strlen("B")] = '\0';
	strncpy(header.channels[1].name, "G", 255);
	header.channels[1].name[strlen("G")] = '\0';
	strncpy(header.channels[2].name, "R", 255);
	header.channels[2].name[strlen("R")] = '\0';

	header.pixel_types = (int *) malloc(sizeof(int) * header.num_channels);
	header.requested_pixel_types = (int *) malloc(sizeof(int) * header.num_channels);
	for (int i = 0; i < header.num_channels; i++) {
		header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
		header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_HALF; // pixel type of output image to be stored in .EXR
	}

	const char *err = nullptr; // or nullptr in C++11 or later.
	int ret = SaveEXRImageToFile(&image, &header, outfilename, &err);
	if (ret != TINYEXR_SUCCESS) {
		std::cout << "Save EXR err: " << err << std::endl;
		FreeEXRErrorMessage(err); // free's buffer for an error message
		return false;
	}
	return true;
}