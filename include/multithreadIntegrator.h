//
// Created by 何振邦 on 2019-08-14.
//

#ifndef HJRT_MULTITHREADINTEGRATOR_H
#define HJRT_MULTITHREADINTEGRATOR_H

#include "integrator.h"
#include <thread>
#include <mutex>

constexpr int MAX_THREAD_NUM = 128;

struct MultiThreadIntegrator : public Integrator {
	int renderThreadNum;
	int runningThreadNum;//actual thread num
	int renderPortionBlock;
	//thread state
	float blockProgress[MAX_THREAD_NUM];

	std::vector<std::shared_ptr<std::thread>> threads;
private:
	std::mutex taskMutex;
	std::vector<std::pair<glm::ivec2, glm::ivec2>> taskList;
	int nextTaskIndex;
public:
	void render(Camera &camera, Scene &scene) override;

	virtual void renderPerformer(int threadNum, Camera &camera, Scene &scene);

	virtual void renderBlock(float &blockProgress, Camera &camera, Scene &scene, glm::ivec2 start, glm::ivec2 end) = 0;

	//derived functions
	bool isFinished() const override;

	float totalProgress() const override;
};

#endif //HJRT_MULTITHREADINTEGRATOR_H
