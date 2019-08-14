//
// Created by 何振邦 on 2019-06-07.
//

#include "pathtracer.h"

PathTracer::PathTracer() {
	maxBounce = 25;
	RRCutBounce = 5;
	state = Integrator::IDLE;
	samplingTex.setSamples(20);
	antiAliasNum = 2;
	renderThreadNum = 0;
	renderPortionBlock = 8;
	latestRenderSec = 99999999;
}
void PathTracer::renderBlock(float &blockProgress, Camera &camera, Scene &scene, glm::ivec2 start, glm::ivec2 end) {
	float subR = 1.0 / antiAliasNum;
	float subS = (-antiAliasNum / 2 + 0.5) * subR;
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
//							color = color + deNanInf(shade(scene, camera.castRay(j + subS + subR * u, i + subS + subR * v), 1));
					}

				}
			}
			color = color * (1.0f / (antiAliasNum * antiAliasNum * actualSamples));
			camera.film.setPixel(i, j, color);
		}
		blockProgress = static_cast<float>(i + 1 - start.y) / (end.y - start.y);
	}
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
			//compute self emission
			glm::vec3 emission = hitInfo.hitobject->material->emitted(ray, hitInfo.uv);
			//compute indirect illumination
			float RRWeight = 1.0f;//Russian roulette weight
			if (depth > RRCutBounce) {
				const glm::vec3 &f = hitInfo.hitobject->material->albedo(hitInfo.uv);
				//Set Russian roulette probability as max color contribution
				float RRProbability = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y : f.z;

				if (random0_1f() > RRProbability) {
					emissionHistory[depth] = emission;
					break;
				} else {
					RRWeight /= RRProbability;
				}
			}
			glm::vec3 attenuation;
			Ray newRay;
			if (hitInfo.hitobject->material->scatterPro(ray, hitInfo, &attenuation, &newRay)) {
				attenuationHistory[depth] = attenuation * RRWeight;
				emissionHistory[depth] = emission;
				ray = newRay;
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
		glm::vec3 emission = hitInfo.hitobject->material->emitted(ray, hitInfo.uv);
		float RRWeight = 1.0f;//Russian roulette weight
		if (depth > RRCutBounce) {
			const glm::vec3 &f = hitInfo.hitobject->material->albedo(hitInfo.uv);
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

std::string PathTracer::getInfo(std::string para) const {
	if (para == "samples") {
		if (samplingTex.getUniformSamples() >= 1)return std::to_string(samplingTex.getUniformSamples());
		else return std::string("adaptive");
	} else if (para == "antialias")return std::to_string(antiAliasNum);
	else if (para == "thread")return std::to_string(runningThreadNum);
	else if (para == "integrator")return std::string("PathTracer");
	return std::string("none");
}