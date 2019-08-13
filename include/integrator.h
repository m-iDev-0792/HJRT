//
// Created by 何振邦 on 2019-06-06.
//

#ifndef HJRT_RENDERER_H
#define HJRT_RENDERER_H

#include "scene.h"
#include "film.h"
#include "camera.h"

struct Integrator {
	enum {
		IDLE,
		RENDERING
	};
	int state;
	int latestRenderSec;
	virtual bool isFinished() const=0;

	virtual std::string getInfo(std::string para)const=0;

	virtual float totalProgress()const =0;

	//Plan: modify "render" to render(Camera &camera, Scene &scene, *!* RenderInfo info*!*)
	//we store some rendering info in RenderInfo class, and pass it to scene for preprocess before rendering
	virtual void render(Camera &camera, Scene &scene) = 0;
};

#endif //HJRT_RENDERER_H
