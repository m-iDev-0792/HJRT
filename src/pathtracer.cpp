//
// Created by 何振邦 on 2019-06-07.
//

#include "pathtracer.h"

void PathTracer::render(Film &film, Camera camera, Scene &scene) {

	if (renderThreads == 1) {
		vector<vec2> startList, endList;
		startList.push_back(vec2(0, 0));
		endList.push_back(vec2(camera.width, camera.height));
		bool finishFlag = false;
		auto start = chrono::high_resolution_clock::now();
		renderPerformer(0, startList, endList, film, camera, scene, &finishFlag);
		auto end = chrono::high_resolution_clock::now();
		auto secDura = chrono::duration_cast<chrono::seconds>(end - start);
		cout << "total rendering time: " << secDura.count() << "s" << endl;
	} else {
		int runThreads = renderThreads > 1 ? renderThreads : thread::hardware_concurrency();
		bool *finishFlag = new bool[runThreads];
		int blockNum = renderPortionBlock;
		int widthBlock = camera.width / blockNum;
		int heightBlock = camera.height / blockNum;
		int totalStep = 1, maxStep = blockNum * blockNum;
		vec2 dir[4]{vec2(widthBlock, 0), vec2(0, -heightBlock), vec2(-widthBlock, 0), vec2(0, heightBlock)};
		vec2 lastStart(blockNum % 2 ? widthBlock * (blockNum / 2) : widthBlock * (blockNum / 2 - 1),
		               heightBlock * (blockNum / 2));
		vector<vec2> blockStarts;
		vector<vector<vec2>> startList, endList;
		startList.resize(runThreads);
		endList.resize(runThreads);
		startList[0].push_back(lastStart);
		endList[0].push_back(vec2(lastStart.x + widthBlock > camera.width ? camera.width : lastStart.x + widthBlock,
		                          lastStart.y + heightBlock > camera.height ? camera.height : lastStart.y +
		                                                                                      heightBlock));
		//gyrate rendering:
		//block:   0 1 2 3 4 5 6 7 8....
		//step:    1 1 2 2 3 3 4 4 5....
		//dir:     → ↓ ← ↑ → ↓ ← ↑ →....
		//   +------>e
		//   | +-----+
		//   | | s-+ |
		//   | |   | |
		//   | +---+ |
		//   +-------+
		for (int d = 0; totalStep < maxStep; ++d) {
			int dirStep = d / 2 + 1;
			for (int s = 0; s < dirStep; ++s) {
				lastStart += dir[d % 4];
				blockStarts.push_back(lastStart);
				startList[totalStep % runThreads].push_back(lastStart);
				endList[totalStep % runThreads].push_back(
						vec2(lastStart.x + widthBlock > camera.width ? camera.width : lastStart.x + widthBlock,
						     lastStart.y + heightBlock > camera.height ? camera.height : lastStart.y + heightBlock));
				if (++totalStep >= maxStep)break;
			}
		}
		vector<shared_ptr<thread>> rt;
		rt.resize(runThreads);

		for (int i = 0; i < runThreads; ++i) {
			auto flag = finishFlag + i;
			*(flag) = false;
			rt[i] = make_shared<thread>(&PathTracer::renderPerformer, this, i, startList[i], endList[i], ref(film),
			                            camera, ref(scene), flag);
			rt[i]->detach();
		}
		auto start = chrono::high_resolution_clock::now();

		//wait all rendering threads complete
		while (true) {
			bool finish = true;
			for (int i = 0; i < runThreads; ++i) {
				finish = finish && *(finishFlag + i);
			}
			if (finish)break;
			this_thread::sleep_for(100ms);
		}

		auto end = chrono::high_resolution_clock::now();
		auto secDura = chrono::duration_cast<chrono::seconds>(end - start);
		cout << "total rendering time: " << secDura.count() << "s" << endl;

		//do some clean jobs
		delete [] finishFlag;
	}

}

void PathTracer::renderPerformer(int threadIndex, vector<vec2> startList, vector<vec2> endList, Film &film,
                                 Camera camera, Scene &scene,
                                 bool *finishFlag) {
	float subR = 1.0 / antiAliasNum;
	float subS = (-antiAliasNum / 2 + 0.5) * subR;

	for (int p = 0; p < startList.size(); ++p) {
		vec2 start = startList[p];
		vec2 end = endList[p];
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
		}
		cout << "thread " << threadIndex << " progress = " << ((p + 1) * 100.0f / startList.size()) << "%" << endl;
	}

	*finishFlag = true;
}