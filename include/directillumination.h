//
// Created by 何振邦 on 2019-08-13.
//

#ifndef HJRT_DIRECTILLUMINATION_H
#define HJRT_DIRECTILLUMINATION_H

#include "multithreadIntegrator.h"
#include "material.h"
#include <thread>
#include <mutex>

struct DirectIllumination : MultiThreadIntegrator {
	SamplingTexture samplingTex;
	int antiAliasNum;
public:
	DirectIllumination();

	glm::vec3 shade(const Scene &_scene, const Ray &_ray);//non-recursive shade function

	void renderBlock(float &blockProgress, Camera &camera, Scene &scene, glm::ivec2 start, glm::ivec2 end) override;

	//derived functions
	std::string getInfo(std::string para) const override;
};

#endif //HJRT_DIRECTILLUMINATION_H
