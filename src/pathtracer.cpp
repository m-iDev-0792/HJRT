//
// Created by 何振邦 on 2019-06-07.
//

#include "pathtracer.h"

PathTracer::PathTracer() {
	state = Integrator::IDLE;
	samples = 20;
	antiAliasNum = 2;
	renderThreadNum = 0;
	renderPortionBlock = 8;
	latestRenderSec = 10e10;
}

void PathTracer::render(Film &film, Camera camera, Scene &scene) {

	//set up parameters
	runningThreadNum = renderThreadNum > 0 ? renderThreadNum : std::thread::hardware_concurrency();
	int blockNum = renderPortionBlock;
	int widthBlock = camera.width / blockNum;
	int heightBlock = camera.height / blockNum;
	int totalStep = 1, maxStep = blockNum * blockNum;

	glm::vec2 dir[4]{glm::vec2(widthBlock, 0), glm::vec2(0, -heightBlock), glm::vec2(-widthBlock, 0),
	                 glm::vec2(0, heightBlock)};
	glm::vec2 lastStart(blockNum % 2 ? widthBlock * (blockNum / 2) : widthBlock * (blockNum / 2 - 1),
	                    heightBlock * (blockNum / 2));

	//init taskList
	taskList.clear();
	idleTaskNum = 0;
	taskList.push_back(std::make_pair(lastStart, glm::vec2(
			lastStart.x + widthBlock > camera.width ? camera.width : lastStart.x + widthBlock,
			lastStart.y + heightBlock > camera.height ? camera.height : lastStart.y +
			                                                            heightBlock)));
	//gyrate block rendering:
	//block:   0 1 2 3 4 5 6 7 8....
	//step:    1 1 2 2 3 3 4 4 5....
	//dir:     → ↓ ← ↑ → ↓ ← ↑ →....
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
			taskList.push_back(std::make_pair(lastStart, glm::vec2(
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
		threads[i] = std::make_shared<std::thread>(&PathTracer::renderPerformer, this, i, ref(film), camera,
		                                           ref(scene));
		threads[i]->detach();
	}
}

void PathTracer::renderPerformer(int threadIndex, Film &film, Camera camera, Scene &scene) {
	float subR = 1.0 / antiAliasNum;
	float subS = (-antiAliasNum / 2 + 0.5) * subR;
	auto startTime = std::chrono::high_resolution_clock::now();

	glm::vec2 start;
	glm::vec2 end;
	for (;;) {
		taskMutex.lock();
		if (idleTaskNum >= taskList.size()) {
			taskMutex.unlock();
			break;
		} else {
			start = taskList[idleTaskNum].first;
			end = taskList[idleTaskNum].second;
			++idleTaskNum;
			taskMutex.unlock();
		}

		blockProgress[threadIndex] = 0;
		for (int i = start.y; i < end.y; ++i) {//row
			for (int j = start.x; j < end.x; ++j) {//column
				vec3 color;
				for (int u = 0; u < antiAliasNum; ++u) {
					for (int v = 0; v < antiAliasNum; ++v) {

						for (int s = 0; s < samples; ++s)
							color = color + scene.shade(camera.getRay(j + subS + subR * u, i + subS + subR * v), 1);

					}
				}
				color = color * (1.0f / (antiAliasNum * antiAliasNum * samples));
				gammaCorrection(color);
				color *= 255;
				toInt(color);
				film.setPixel(i, j, color);
			}
			blockProgress[threadIndex] = static_cast<float>(i + 1 - start.y) / (end.y - start.y);
		}
		blockProgress[threadIndex] = 1;
	}
	//finished
	auto endTime = std::chrono::high_resolution_clock::now();
	auto secDura = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
	if (secDura.count() > latestRenderSec)latestRenderSec = secDura.count();
	if (isFinish())state = Integrator::IDLE;
	std::cout<<"thread "<<threadIndex<<" finished"<<endl;
}

bool PathTracer::isFinish() {
	if (idleTaskNum < taskList.size())return false;
	for (int i = 0; i < runningThreadNum; ++i) {
		if (blockProgress[i] < 0.999)return false;
	}
	return true;
}

float PathTracer::totalProgress() {
	float t;
	return (t = static_cast<float>(idleTaskNum) / taskList.size()) > 1 ? 1 : t;
}