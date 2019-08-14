//
// Created by 何振邦 on 2019-08-14.
//

#include "multithreadIntegrator.h"

void MultiThreadIntegrator::render(Camera &camera, Scene &scene) {
	//initialize scene, it may take a while
	scene.prepareRendering();

	//set up parameters
	runningThreadNum = renderThreadNum > 0 ? renderThreadNum : std::thread::hardware_concurrency();
	int blockNum = renderPortionBlock;
	int widthBlock = camera.width / blockNum;
	int heightBlock = camera.height / blockNum;
	int totalStep = 1, maxStep = blockNum * blockNum;

	glm::ivec2 dir[4]{glm::ivec2(widthBlock, 0), glm::ivec2(0, -heightBlock), glm::ivec2(-widthBlock, 0),
	                  glm::ivec2(0, heightBlock)};
	glm::ivec2 lastStart(blockNum % 2 ? widthBlock * (blockNum / 2) : widthBlock * (blockNum / 2 - 1),
	                     heightBlock * (blockNum / 2));

	//init taskList
	taskList.clear();
	nextTaskIndex = 0;
	taskList.push_back(std::make_pair(lastStart, glm::ivec2(
			lastStart.x + widthBlock > camera.width ? camera.width : lastStart.x + widthBlock,
			lastStart.y + heightBlock > camera.height ? camera.height : lastStart.y +
			                                                            heightBlock)));
	//spiral block rendering:
	//d:        0 1 2 3 4 5 6 7 8....
	//dirStep:  1 1 2 2 3 3 4 4 5....
	//dir:      → ↓ ← ↑ → ↓ ← ↑ →....
	//   +------>e
	//   | +-----+
	//   | | s-+ |
	//   | |   | |
	//   | +---+ |
	//   +-------+  s=start  e=end
	for (int d = 0; totalStep < maxStep; ++d) {
		int dirStep = d / 2 + 1;
		for (int s = 0; s < dirStep; ++s) {
			lastStart += dir[d % 4];
			taskList.push_back(std::make_pair(lastStart, glm::ivec2(
					lastStart.x + widthBlock > camera.width ? camera.width : lastStart.x + widthBlock,
					lastStart.y + heightBlock > camera.height ? camera.height : lastStart.y +
					                                                            heightBlock)));
			if (++totalStep >= maxStep)break;
		}
	}

	//allocate threads and perform actual rendering
	threads.clear();
	threads.resize(runningThreadNum);
	state = Integrator::RENDERING;

	for (int i = 0; i < runningThreadNum; ++i) {
		threads[i] = std::make_shared<std::thread>(&MultiThreadIntegrator::renderPerformer, this, i, std::ref(camera),
		                                           std::ref(scene));
		threads[i]->detach();
	}
}

void MultiThreadIntegrator::renderPerformer(int threadNum, Camera &camera, Scene &scene) {
	auto startTime = std::chrono::high_resolution_clock::now();

	glm::ivec2 start;
	glm::ivec2 end;
	for (;;) {
		taskMutex.lock();
		if (nextTaskIndex >= taskList.size()) {
			taskMutex.unlock();
			break;
		} else {
			start = taskList[nextTaskIndex].first;
			end = taskList[nextTaskIndex].second;
			++nextTaskIndex;
			taskMutex.unlock();
		}
		blockProgress[threadNum] = 0;
		renderBlock(blockProgress[threadNum], camera, scene, start, end);
		blockProgress[threadNum] = 1;
	}
	//finished
	auto endTime = std::chrono::high_resolution_clock::now();
	auto secDura = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
	latestRenderSec = secDura.count();
	if (isFinished())state = Integrator::IDLE;
	std::cout << "thread " << threadNum << " finished" << std::endl;
}

bool MultiThreadIntegrator::isFinished() const {
	if (nextTaskIndex < taskList.size())return false;
	for (int i = 0; i < runningThreadNum; ++i) {
		if (blockProgress[i] < 0.999)return false;
	}
	return true;
}

float MultiThreadIntegrator::totalProgress() const {
	float t = nextTaskIndex - runningThreadNum;
	for (int i = 0; i < runningThreadNum; ++i)t += blockProgress[i];
	return (t /= taskList.size()) > 1 ? 1 : t;
}