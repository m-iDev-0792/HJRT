//
// Created by 何振邦 on 2019-06-06.
//

#ifndef HJRT_PATHTRACER_H
#define HJRT_PATHTRACER_H

#include "integrator.h"
#include <thread>
constexpr int MAX_THREAD=128;
struct PathTracer : Integrator {
	int samples;
	int antiAliasNum;
	int renderThreadNum;
	int runningThreadNum;//actual thread num
	int renderPortionBlock;
	int latestRenderSec;
	//thread state
	float blockProgress[MAX_THREAD];
	float threadTotalProgress[MAX_THREAD];
	bool finishFlag[MAX_THREAD];
	std::vector<std::shared_ptr<std::thread>> threads;
public:
	PathTracer();

	void render(Film &film, Camera camera, Scene &scene) override;

	void renderPerformer(int threadNum, std::vector<glm::vec2> start, std::vector<glm::vec2> end, Film &film, Camera camera,
	                     Scene &scene);

	bool isFinish();
};

#endif //HJRT_PATHTRACER_H
