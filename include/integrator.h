//
// Created by 何振邦 on 2019-06-06.
//

#ifndef HJRT_RENDERER_H
#define HJRT_RENDERER_H

#include "scene.h"
#include "film.h"
#include "camera.h"

struct Integrator{
	enum {
		IDLE,
		RENDERING
	};
	int state;
	//Plan: modify "render" to render(Film &image, Camera camera, Scene &scene, *!* RenderInfo info*!*)
	//we store some rendering info in RenderInfo class, and pass it to scene for preprocess before rendering
	virtual void render(Film &image, Camera camera, Scene &scene) =0;
};
#endif //HJRT_RENDERER_H
