//
// Created by 何振邦 on 2019-08-13.
//

#ifndef HJRT_PATHTRACERDI_H
#define HJRT_PATHTRACERDI_H

#include "integrator.h"
#include "material.h"
#include <thread>
#include <mutex>

constexpr int MAX_THREAD_PTDI = 128;

struct PathTracerDI : Integrator {
	SamplingTexture samplingTex;
	int antiAliasNum;
	int maxBounce;
	int RRCutBounce;


	int renderThreadNum;
	int runningThreadNum;//actual thread num
	int renderPortionBlock;
	//thread state
	float blockProgress[MAX_THREAD_PTDI];

	std::vector<std::shared_ptr<std::thread>> threads;
private:
	std::mutex taskMutex;
	std::vector<std::pair<glm::vec2, glm::vec2>> taskList;
	int idleTaskNum;
public:
	PathTracerDI();

	glm::vec3 shade(const Scene &_scene, const Ray &_ray);//non-recursive shade function

	glm::vec3 shade(const Scene &scene, const Ray &ray, int depth);//recursive shade function

	void render(Camera &camera, Scene &scene) override;

	void renderPerformer(int threadNum, Camera &camera, Scene &scene);

	//derived functions
	bool isFinished()const override ;

	float totalProgress()const override ;

	std::string getInfo(std::string para)const override ;
};
#endif //HJRT_PATHTRACERDI_H
