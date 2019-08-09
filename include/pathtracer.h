//
// Created by 何振邦 on 2019-06-06.
//

#ifndef HJRT_PATHTRACER_H
#define HJRT_PATHTRACER_H

#include "integrator.h"
#include "material.h"
#include <thread>
#include <mutex>

constexpr int MAX_THREAD = 128;

struct PathTracer : Integrator {
	SamplingTexture samplingTex;
	int antiAliasNum;
	int maxBounce;
	int RRCutBounce;


	int renderThreadNum;
	int runningThreadNum;//actual thread num
	int renderPortionBlock;
	int latestRenderSec;
	//thread state
	float blockProgress[MAX_THREAD];

	std::vector<std::shared_ptr<std::thread>> threads;
private:
	std::mutex taskMutex;
	std::vector<std::pair<glm::vec2, glm::vec2>> taskList;
	int idleTaskNum;
public:
	PathTracer();

	glm::vec3 shade(const Scene &_scene, const Ray &_ray);//non-recursive shade function

	glm::vec3 shade(const Scene &scene, const Ray &ray, int depth);//recursive shade function

	void render(Camera &camera, Scene &scene) override;

	void renderPerformer(int threadNum, Camera &camera, Scene &scene);

	bool isFinish();

	float totalProgress();
};

#endif //HJRT_PATHTRACER_H
