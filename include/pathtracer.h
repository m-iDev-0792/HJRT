//
// Created by 何振邦 on 2019-06-06.
//

#ifndef HJRT_PATHTRACER_H
#define HJRT_PATHTRACER_H

#include "integrator.h"
#include <thread>
#include <mutex>
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

	std::vector<std::shared_ptr<std::thread>> threads;
private:
	std::mutex taskMutex;
	std::vector<std::pair<glm::vec2,glm::vec2>> taskList;
	int idleTaskNum;
public:
	PathTracer();

	void render(Film &film, Camera camera, Scene &scene) override;

	void renderPerformer(int threadNum, Film &film, Camera camera,Scene &scene);

	bool isFinish();

	float totalProgress();
};

#endif //HJRT_PATHTRACER_H
