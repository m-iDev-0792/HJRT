//
// Created by 何振邦 on 2019-06-06.
//

#ifndef HJRT_PATHTRACER_H
#define HJRT_PATHTRACER_H

#include "multithreadIntegrator.h"
#include "material.h"
struct PathTracer : MultiThreadIntegrator {
	SamplingTexture samplingTex;
	int antiAliasNum;
	int maxBounce;
	int RRCutBounce;

	PathTracer();

	glm::vec3 shade(const Scene &_scene, const Ray &_ray);//non-recursive shade function

	glm::vec3 shade(const Scene &scene, const Ray &ray, int depth);//recursive shade function

	void renderBlock(float &blockProgress, Camera &camera, Scene &scene, glm::ivec2 start, glm::ivec2 end) override;

	std::string getInfo(std::string para)const override ;
};

#endif //HJRT_PATHTRACER_H
