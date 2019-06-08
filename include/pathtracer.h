//
// Created by 何振邦 on 2019-06-06.
//

#ifndef HJRT_PATHTRACER_H
#define HJRT_PATHTRACER_H

#include "integrator.h"
#include <thread>

struct PathTracer : Integrator {
	int samples;
	int antiAliasNum;
	int renderThreads;
	int renderPortionBlock;

	void render(Film &film, Camera camera, Scene &scene) override;

	void renderPerformer(int threadNum, std::vector<glm::vec2> start, std::vector<glm::vec2> end, Film &film, Camera camera,
	                     Scene &scene, bool *finishFlag);
};

#endif //HJRT_PATHTRACER_H
