//
// Created by 何振邦 on 2019-06-07.
//

#include "pathtracer.h"
PathTracer::PathTracer(){
	state=Integrator::IDLE;
	samples=20;
	antiAliasNum=2;
	renderThreadNum=0;
	renderPortionBlock=8;
	latestRenderSec=10e10;
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
	std::vector<glm::vec2> blockStarts;
	std::vector<std::vector<glm::vec2>> startList, endList;
	startList.resize(runningThreadNum);
	endList.resize(runningThreadNum);
	startList[0].push_back(lastStart);
	endList[0].push_back(glm::vec2(lastStart.x + widthBlock > camera.width ? camera.width : lastStart.x + widthBlock,
	                               lastStart.y + heightBlock > camera.height ? camera.height : lastStart.y +
	                                                                                           heightBlock));
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
			blockStarts.push_back(lastStart);
			startList[totalStep % runningThreadNum].push_back(lastStart);
			endList[totalStep % runningThreadNum].push_back(
					glm::vec2(lastStart.x + widthBlock > camera.width ? camera.width : lastStart.x + widthBlock,
					          lastStart.y + heightBlock > camera.height ? camera.height : lastStart.y + heightBlock));
			if (++totalStep >= maxStep)break;
		}
	}


	//allocate threads and perform actual rendering
	threads.clear();
	threads.resize(runningThreadNum);
	state=Integrator::RENDERING;
	for (int i = 0; i < runningThreadNum; ++i) {
		finishFlag[i] = false;
		threads[i] = std::make_shared<std::thread>(&PathTracer::renderPerformer, this, i, startList[i], endList[i],
		                                           ref(film),
		                                           camera, ref(scene));
		threads[i]->detach();
	}

	//block code and wait all rendering threads complete
//	while (true) {
//		bool finish = true;
//		for (int i = 0; i < runningThreadNum; ++i) {
//			finish = finish && finishFlag[i];
//		}
//		if (finish)break;
//		std::this_thread::sleep_for(100ms);
//	}
}

void PathTracer::renderPerformer(int threadIndex, std::vector<glm::vec2> startList, std::vector<glm::vec2> endList,
                                 Film &film,
                                 Camera camera, Scene &scene) {

	float subR = 1.0 / antiAliasNum;
	float subS = (-antiAliasNum / 2 + 0.5) * subR;
	auto start = std::chrono::high_resolution_clock::now();
	threadTotalProgress[threadIndex]=0;
	for (int p = 0; p < startList.size(); ++p) {

		glm::vec2 start = startList[p];
		glm::vec2 end = endList[p];
		blockProgress[threadIndex]=0;

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
			blockProgress[threadIndex]= static_cast<float>(i+1-start.y)/(end.y-start.y);
			threadTotalProgress[threadIndex]= (p+blockProgress[threadIndex])/startList.size();
		}
	}
	//finished
	auto end = std::chrono::high_resolution_clock::now();
	auto secDura = std::chrono::duration_cast<std::chrono::seconds>(end - start);
	if (secDura.count() > latestRenderSec)latestRenderSec = secDura.count();
	blockProgress[threadIndex]=threadTotalProgress[threadIndex]=1;
	finishFlag[threadIndex] = true;
	if(isFinish())state=Integrator::IDLE;
}

bool PathTracer::isFinish() {
	bool finish = true;
	for (int i = 0; i < runningThreadNum; ++i) {
		finish = finish && finishFlag[i];
	}
	return finish;
}