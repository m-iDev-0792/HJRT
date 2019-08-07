//
// Created by 何振邦 on 2019-08-07.
//

#include <iostream>
#include <vector>
#include "geometry.h"
#include "movingobject.h"
#include "mesh.h"
#include "pathtracer.h"
#include "rtui.h"

using namespace std;
using namespace glm;

int main() {
	Camera camera(vec3(33, 20, 28), vec3(-0.85, -0.2, -0.85), 512 / 2 + 128, 512 / 2, 60);

	Scene scene;
	scene.useBVH = false;
	scene.ambient = vec3(0);
	scene.shutterPeriod = TimePeriod(0, 1.0);

	auto castle=make_shared<Mesh>("Castle.obj");
	cout << "loaded model: " <<castle->name << "  triangle num: " << castle->triangles.size() << endl;
	scene.addShape(castle);

	auto sky=make_shared<Sphere>(vec3(0),200);
	sky->setMaterial(make_shared<EnvironmentMap>(make_shared<ImageTexture>("sky.jpg")));
	scene.addShape(sky);


	Film image(camera.width, camera.height);

	const int antiAliasNum = 2;
	const int samples = 200;


	//----------------------Render---------------------------
	PathTracer path;
	path.antiAliasNum = antiAliasNum;
	path.renderThreadNum = 4;
	path.samples = samples;
	path.renderPortionBlock = 8;//path.renderThreadNum;

	path.render(image, camera, scene);


	RTUI ui(800, 600, "HJRT");
	ui.film = &image;
	ui.pathTracer = &path;
	ui.run();
	cout << "rendering time:" << secondToFormatTime(path.latestRenderSec) << endl;
	glfwTerminate();

	//----------------------Write image----------------------
	image.save("image.png");
	cout << "finished" << endl;
	return 0;
}