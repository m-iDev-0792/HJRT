//
// Created by 何振邦 on 2019-08-13.
//

#include "explicitlighting.h"

ExplicitLightingPathTracer::ExplicitLightingPathTracer() {
	maxBounce = 25;
	RRCutBounce = 5;
	state = Integrator::IDLE;
	samplingTex.setSamples(20);
	antiAliasNum = 2;
	renderThreadNum = 0;
	renderPortionBlock = 8;
	latestRenderSec = 99999999;
}

void ExplicitLightingPathTracer::renderBlock(float &blockProgress, Camera &camera, Scene &scene, glm::ivec2 start, glm::ivec2 end) {
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

glm::vec3 ExplicitLightingPathTracer::shade(const Scene &_scene, const Ray &_ray) {
	int depth = 0;
	glm::vec3 *attenuationHistory = new glm::vec3[maxBounce + 2];
	glm::vec3 *directIlluminationHistory = new glm::vec3[maxBounce + 2];
	Ray ray = _ray;
	int emissionFlag = 0;
	for (;;) {
		if (depth > maxBounce) {
			directIlluminationHistory[depth] = _scene.ambient;
			break;
		}

		HitInfo hitInfo;
		if (_scene.intersect(ray, &hitInfo)) {
			glm::vec2 uv;
			bool hasUV = hitInfo.hitobject->getUV(hitInfo, &uv);
			hitInfo.uv = uv;
			//compute self emission
			glm::vec3 emission = hitInfo.hitobject->material->emitted(ray, hitInfo.uv);

			//compute direct illumination
			glm::vec3 directIllumination(0);
			float contributDirectLightNum = 0;
			for (int i = 0; i < _scene.lights.size(); ++i) {
				std::shared_ptr<SampleableShape> lightShape = _scene.lights[i];

				glm::vec3 lightSampleDir;
				float lightPdf = lightShape->sample(hitInfo, &lightSampleDir);
				if (glm::dot(lightSampleDir, hitInfo.normal) < 0.0001)continue;

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
				directIlluminationHistory[depth] = directIllumination / contributDirectLightNum;
			else directIlluminationHistory[depth] = glm::vec3(0);
			//consider self emission?
			if (!emissionFlag) {
				//if hit surface is a non-reflection surface, consider self-emission
				if (!(hitInfo.hitobject->material->type & MATERIAL_TYPE::REFLECTION)) {
					directIlluminationHistory[depth] += hitInfo.hitobject->material->emitted(ray, hitInfo.uv);
					++emissionFlag;
				}//if hit surface is a reflection surface, just ignore
			}
			//compute indirect illumination
			float RRWeight = 1.0f;//Russian roulette weight
			if (depth > RRCutBounce) {
				const glm::vec3 &f = hitInfo.hitobject->material->albedo(hitInfo.uv);
				//Set Russian roulette probability as max color contribution
				float RRProbability = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y : f.z;

				if (random0_1f() > RRProbability) {
					break;
				} else {
					RRWeight /= RRProbability;
				}
			}
			//generate a new ray
			glm::vec3 attenuation;
			Ray newRay;
			if (hitInfo.hitobject->material->scatterPro(ray, hitInfo, &attenuation, &newRay)) {
				//we have a valid scatter ray
				attenuationHistory[depth] = attenuation * RRWeight;
				ray = newRay;
				++depth;
				continue;
			} else {
				//no scatter happens on the surface
				break;
			}
		} else {
			//ray dosen't hit anything
			directIlluminationHistory[depth] = _scene.ambient;//direct light
			break;
		}
	}
	glm::vec3 color = directIlluminationHistory[depth];
	for (int i = depth - 1; i >= 0; --i) {
		color = attenuationHistory[i] * color + directIlluminationHistory[i];
	}

	delete[] attenuationHistory;
	delete[] directIlluminationHistory;
	return color;
}

glm::vec3 ExplicitLightingPathTracer::shade(const Scene &scene, const Ray &ray, int depth) {
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

std::string ExplicitLightingPathTracer::getInfo(std::string para) const {
	if (para == "samples") {
		if (samplingTex.getUniformSamples() > 1)return std::to_string(samplingTex.getUniformSamples());
		else return std::string("adaptive");
	} else if (para == "antialias")return std::to_string(antiAliasNum);
	else if (para == "thread")return std::to_string(runningThreadNum);
	else if (para == "integrator")return std::string("PathTracer\n(Explicit Lighting)");
	return std::string("none");
}