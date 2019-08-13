//
// Created by 何振邦 on 2019-08-13.
//

#include "directillumination.h"

DirectIllumination::DirectIllumination() {
	state = Integrator::IDLE;
	samplingTex.setSamples(20);
	antiAliasNum = 2;
	renderThreadNum = 0;
	renderPortionBlock = 8;
	latestRenderSec = 99999999;
}

void DirectIllumination::render(Camera &camera, Scene &scene) {
	//initialize scene, it may take a while
	scene.prepareRendering();

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
		threads[i] = std::make_shared<std::thread>(&DirectIllumination::renderPerformer, this, i, std::ref(camera),
		                                           std::ref(scene));
		threads[i]->detach();
	}
}

void DirectIllumination::renderPerformer(int threadNum, Camera &camera, Scene &scene) {
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

		blockProgress[threadNum] = 0;
		for (int i = start.y; i < end.y; ++i) {//row
			for (int j = start.x; j < end.x; ++j) {//column
				glm::vec3 color(0);
				int actualSamples;
				for (int u = 0; u < antiAliasNum; ++u) {
					for (int v = 0; v < antiAliasNum; ++v) {
						actualSamples = samplingTex.getColor(glm::vec2(j + subS + subR * u, i + subS + subR * v));
						for (int s = 0; s < actualSamples; ++s) {
							color = color + deNanInf(shade(scene,
							                               camera.castRay(j + subS + subR * u, i + subS + subR * v,
							                                              scene.shutterPeriod)));
						}

					}
				}
				color = color * (1.0f / (antiAliasNum * antiAliasNum * actualSamples));
				camera.film.setPixel(i, j, color);
			}
			blockProgress[threadNum] = static_cast<float>(i + 1 - start.y) / (end.y - start.y);
		}
		blockProgress[threadNum] = 1;
	}
	//finished
	auto endTime = std::chrono::high_resolution_clock::now();
	auto secDura = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
	latestRenderSec = secDura.count();
	if (isFinished())state = Integrator::IDLE;
	std::cout << "thread " << threadNum << " finished" << std::endl;
}

glm::vec3 DirectIllumination::shade(const Scene &_scene, const Ray &_ray) {
	int depth = 0;
	glm::vec3 emission(0);
	glm::vec3 totalDirectIllumination(0);
	Ray ray = _ray;

	HitInfo hitInfo;
	if (_scene.intersect(ray, &hitInfo)) {
		glm::vec2 uv;
		bool hasUV = hitInfo.hitobject->getUV(hitInfo, &uv);
		hitInfo.uv = uv;
		//compute self emission
		emission = hitInfo.hitobject->material->emitted(ray, hitInfo.uv);

		//compute direct illumination
		const int lightNum = _scene.lights.size();
		glm::vec3 directIllumination(0);
		float contributDirectLightNum = 0;
		for (int i = 0; i < lightNum; ++i) {
			std::shared_ptr<SampleableShape> lightShape = _scene.lights[i];
			glm::vec3 lightSampleDir;
			float lightPdf = lightShape->sample(hitInfo, &lightSampleDir);
			if (glm::dot(lightSampleDir, hitInfo.normal) < 0.00001)continue;
			Ray shadowray(hitInfo.hitpoint, lightSampleDir, ray.time, ray.tMin, ray.tMax);
			HitInfo lightSampleHitInfo;
			if (_scene.intersect(shadowray, &lightSampleHitInfo)) {
				if (lightSampleHitInfo.hitobject != lightShape.get())continue;
				//has direct illumination
				auto material = hitInfo.hitobject->material;
				auto d = lightSampleHitInfo.hitpoint - hitInfo.hitpoint;
				float distance = glm::dot(d, d);
				glm::vec3 a =
						material->brdf(ray.dir, lightSampleDir, hitInfo) * glm::dot(hitInfo.normal, lightSampleDir);
				directIllumination +=
						a * lightShape->material->emitted(shadowray, lightSampleHitInfo.uv) / lightPdf;
				contributDirectLightNum += 1;
			}
		}
		if (contributDirectLightNum > 0)
			totalDirectIllumination = directIllumination / contributDirectLightNum;
		else totalDirectIllumination = glm::vec3(0);

	} else {
		emission = _scene.ambient;
		totalDirectIllumination = glm::vec3(0);//direct light
	}
	return emission + totalDirectIllumination;
}

bool DirectIllumination::isFinished() const {
	if (idleTaskNum < taskList.size())return false;
	for (int i = 0; i < runningThreadNum; ++i) {
		if (blockProgress[i] < 0.999)return false;
	}
	return true;
}

float DirectIllumination::totalProgress() const {
	float t;
	return (t = static_cast<float>(idleTaskNum) / taskList.size()) > 1 ? 1 : t;
}

std::string DirectIllumination::getInfo(std::string para) const {
	if (para == "samples") {
		if (samplingTex.getUniformSamples() > 1)return std::to_string(samplingTex.getUniformSamples());
		else return std::string("adaptive");
	} else if (para == "antialias")return std::to_string(antiAliasNum);
	else if (para == "thread")return std::to_string(runningThreadNum);
	else if (para == "integrator")return std::string("DirectIllumination");
	return std::string("none");
}