//
// Created by 何振邦 on 2019-06-07.
//

#include "pathtracer.h"

PathTracer::PathTracer() {
	maxBounce = 25;
	RRCutBounce = 5;
	state = Integrator::IDLE;
	samples = 20;
	antiAliasNum = 2;
	renderThreadNum = 0;
	renderPortionBlock = 8;
	latestRenderSec = 99999999;
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
		threads[i] = std::make_shared<std::thread>(&PathTracer::renderPerformer, this, i, std::ref(film), camera,
		                                           std::ref(scene));
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
				glm::vec3 color(0);
				for (int u = 0; u < antiAliasNum; ++u) {
					for (int v = 0; v < antiAliasNum; ++v) {

						for (int s = 0; s < samples; ++s) {
							color = color + shade(scene, camera.getRay(j + subS + subR * u, i + subS + subR * v));
//							color = color + shade(scene, camera.getRay(j + subS + subR * u, i + subS + subR * v), 1);
						}

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
	latestRenderSec = secDura.count();
	if (isFinish())state = Integrator::IDLE;
	std::cout << "thread " << threadIndex << " finished" << std::endl;
}

glm::vec3 PathTracer::shade(const Scene &_scene, const Ray &_ray) {
	int depth = 0;
	glm::vec3 *emissionHistory = new glm::vec3[maxBounce + 2];
	glm::vec3 *attenuationHistory = new glm::vec3[maxBounce + 2];
	Ray ray = _ray;
	for (;;) {
		if (depth > maxBounce) {
			emissionHistory[depth] = _scene.ambient;
			break;
		}
		HitInfo hitInfo;
		if (_scene.intersect(ray, &hitInfo)) {
			glm::vec2 uv;
			bool hasUV = hitInfo.hitobject->getUV(hitInfo, &uv);
			hitInfo.uv = uv;
			glm::vec3 emission = hitInfo.hitobject->material->emitted(ray,hitInfo.uv);
			float RRWeight=1.0f;//Russian roulette weight
			if (depth > RRCutBounce) {
				const glm::vec3 &f = hitInfo.hitobject->material->albedoed(hitInfo.uv);
				//Set Russian roulette probability as max color contribution
				float RRProbability = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y : f.z;

				if (random0_1f() > RRProbability) {
					emissionHistory[depth] = emission;
					break;
				}else{
					RRWeight/=RRProbability;
				}
			}
			glm::vec3 attenuation;
			Ray newRay;
			if (hitInfo.hitobject->material->scatter(ray, hitInfo, &attenuation, &newRay)) {
				attenuationHistory[depth] = attenuation*RRWeight;
				emissionHistory[depth] = emission;
				ray=newRay;
				++depth;
				continue;
			} else {
				emissionHistory[depth] = emission;
				break;
			}
		} else {
			emissionHistory[depth] = _scene.ambient;
			break;
		}
	}
	glm::vec3 color = emissionHistory[depth];
	for (int i = depth - 1; i >= 0; --i) {
		color = emissionHistory[i] + attenuationHistory[i] * color;
	}
	delete[] emissionHistory;
	delete[] attenuationHistory;
	return color;
}

glm::vec3 PathTracer::shade(const Scene &scene, const Ray &ray, int depth) {
	if (depth > maxBounce)return scene.ambient;
	HitInfo hitInfo;
	if (scene.intersect(ray, &hitInfo)) {
		glm::vec2 uv;
		bool hasUV = hitInfo.hitobject->getUV(hitInfo, &uv);
		hitInfo.uv = uv;
		glm::vec3 emission = hitInfo.hitobject->material->emitted(ray,hitInfo.uv);
		float RRWeight = 1.0f;//Russian roulette weight
		if (depth > RRCutBounce) {
			const glm::vec3 &f = hitInfo.hitobject->material->albedoed(hitInfo.uv);
			//Set Russian roulette probability as max color contribution
			float RRProbability = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y : f.z;
			if (random0_1f() > RRProbability)return emission;
			else RRWeight /= RRProbability;
		}
		Ray newRay;
		glm::vec3 attenuation;
		if (hitInfo.hitobject->material->scatter(ray, hitInfo, &attenuation, &newRay)) {
			return emission + attenuation * RRWeight * shade(scene, newRay, depth + 1);
		} else
			return emission;
	} else {
		return scene.ambient;
	}
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