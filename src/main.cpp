#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include "objects.h"
#include "mesh.h"
#include "pathtracer.h"
#include "rtui.h"
using namespace std;
using namespace glm;

int main() {
	Camera camera(vec3(0, 2, 14), vec3(0, 0, -1), 512/2, 512/2, 45);

	auto sphere = make_shared<Sphere>(vec3(0, -1, 0), 1);
	sphere->name = "diffuse";
	sphere->material = make_shared<Lambertian>(vec3(230, 173, 104) * (1 / 255.0f));

	auto metalSphere = make_shared<Sphere>(vec3(2, -1, -2), 1);
	metalSphere->name = "metal";
	metalSphere->material = make_shared<Metal>(vec3(0.8), 0);

	auto glassSphere = make_shared<Sphere>(vec3(2, -1, 0), 1);
	glassSphere->name = "glass";
	glassSphere->material = make_shared<Dielectric>(1.5);

	auto ground = make_shared<Sphere>(vec3(0, -2 - 100, 0), 100);
	ground->name = "ground";
	ground->material = make_shared<Lambertian>(vec3(189, 211, 223) * (1 / 255.0f));

	auto light = make_shared<Sphere>(vec3(0, 6, 0), 1);
	light->name = "light";
	light->material = make_shared<Lambertian>(vec3(0));
	light->material->emission = vec3(10);

	const int PlaneR = 4;
	auto white = make_shared<Lambertian>(vec3(0.8));

	auto floor1 = make_shared<Triangle>(vec3(-PlaneR, -2, PlaneR), vec3(PlaneR, -2, -PlaneR),
	                                    vec3(-PlaneR, -2, -PlaneR));
	floor1->name = "floor1";
	floor1->setUVs(vec2(0, 0),vec2(1, 1),vec2(0, 1));

	auto floor2 = make_shared<Triangle>(vec3(-PlaneR, -2, PlaneR), vec3(PlaneR, -2, PlaneR), vec3(PlaneR, -2, -PlaneR));
	floor2->name = "floor2";
	floor2->setUVs(vec2(0, 0),vec2(1, 0),vec2(1, 1));
	floor1->material = floor2->material = white;

	auto greenWall1 = make_shared<Triangle>(vec3(PlaneR, -2, -PlaneR), vec3(PlaneR, -2, PlaneR),
	                                        vec3(PlaneR, -2 + 2 * PlaneR, PlaneR));
	greenWall1->name = "greenWall1";
	auto greenWall2 = make_shared<Triangle>(vec3(PlaneR, -2, -PlaneR), vec3(PlaneR, -2 + 2 * PlaneR, PlaneR),
	                                        vec3(PlaneR, -2 + 2 * PlaneR, -PlaneR));
	greenWall2->name = "greenWall2";
	greenWall1->material = greenWall2->material = make_shared<Lambertian>(
			vec3(0.25, 0.75, 0.25));

	auto redWall1 = make_shared<Triangle>(vec3(-PlaneR, -2, PlaneR), vec3(-PlaneR, -2, -PlaneR),
	                                      vec3(-PlaneR, -2 + 2 * PlaneR, -PlaneR));
	redWall1->name = "redWall1";
	auto redWall2 = make_shared<Triangle>(vec3(-PlaneR, -2, PlaneR), vec3(-PlaneR, -2 + 2 * PlaneR, -PlaneR),
	                                      vec3(-PlaneR, -2 + 2 * PlaneR, PlaneR));
	redWall2->name = "redWall2";
	redWall1->material = redWall2->material = make_shared<Lambertian>(
			vec3(0.75, 0.25, 0.25));

	auto ceil1 = make_shared<Triangle>(vec3(-PlaneR, -2 + 2 * PlaneR, PlaneR), vec3(PlaneR, -2 + 2 * PlaneR, -PlaneR),
	                                   vec3(-PlaneR, -2 + 2 * PlaneR, -PlaneR));
	ceil1->name = "ceil1";
	auto ceil2 = make_shared<Triangle>(vec3(-PlaneR, -2 + 2 * PlaneR, PlaneR), vec3(PlaneR, -2 + 2 * PlaneR, PlaneR),
	                                   vec3(PlaneR, -2 + 2 * PlaneR, -PlaneR));
	ceil2->name = "ceil2";
	ceil1->material = ceil2->material = white;

	auto back1 = make_shared<Triangle>(vec3(-PlaneR, -2, -PlaneR), vec3(PlaneR, -2, -PlaneR),
	                                   vec3(PlaneR, -2 + 2 * PlaneR, -PlaneR));
	back1->name = "back1";
	auto back2 = make_shared<Triangle>(vec3(-PlaneR, -2, -PlaneR), vec3(PlaneR, -2 + 2 * PlaneR, -PlaneR),
	                                   vec3(-PlaneR, -2 + 2 * PlaneR, -PlaneR));
	back2->name = "back2";
	back1->material = back2->material = white;

	auto light1 = make_shared<Triangle>(vec3(-PlaneR / 4, -2.05 + 2 * PlaneR, PlaneR / 4),
	                                    vec3(PlaneR / 4, -2.05 + 2 * PlaneR, -PlaneR / 4),
	                                    vec3(-PlaneR / 4, -2.05 + 2 * PlaneR, -PlaneR / 4));
	light1->name = "light1";
	auto light2 = make_shared<Triangle>(vec3(-PlaneR / 4, -2.05 + 2 * PlaneR, PlaneR / 4),
	                                    vec3(PlaneR / 4, -2.05 + 2 * PlaneR, PlaneR / 4),
	                                    vec3(PlaneR / 4, -2.05 + 2 * PlaneR, -PlaneR / 4));
	light2->name = "light2";
	light1->material = light2->material = light->material;
	mat4 enlarge(1.0f);
	enlarge=scale(enlarge,vec3(1.5,1,1.5));
//	light1->transform(enlarge);light2->transform(enlarge);

	Scene scene;
	scene.useBVH = false;
	scene.ambient = vec3(0);
//	scene.objects.push_back(sphere);
//	scene.objects.push_back(ground);
//	scene.objects.push_back(metalSphere);
	scene.objects.push_back(light1);
	scene.objects.push_back(light2);

	scene.objects.push_back(floor1);
	scene.objects.push_back(floor2);

	scene.objects.push_back(ceil1);
	scene.objects.push_back(ceil2);

	scene.objects.push_back(redWall1);
	scene.objects.push_back(redWall2);

	scene.objects.push_back(greenWall1);
	scene.objects.push_back(greenWall2);

	scene.objects.push_back(back1);
	scene.objects.push_back(back2);


//	auto model=make_shared<Mesh>();
//	model->transMat=scale(model->transMat,vec3(1.5f));
//	model->transMat=translate(model->transMat,vec3(0,0.35,0));
//	model->material=glassSphere->material;
//	model->loadMesh("../mesh/bunny.obj");
//	cout<<"loaded model: "<<model->name<<"  triangle num: "<<model->triangles.size()<<endl;
//	scene.objects.push_back(model);

	auto huaji=make_shared<Mesh>();
	huaji->material=make_shared<Dielectric>(2.417);
	huaji->transMat=translate(huaji->transMat,vec3(0,-2,0));
	huaji->loadMesh("../mesh/diamondStanding.obj");
	cout<<"loaded model: "<<huaji->name<<"  triangle num: "<<huaji->triangles.size()<<endl;
	scene.objects.push_back(huaji);

//	auto fogBoundary=make_shared<Sphere>(vec3(0,0,0),2);
//	auto fog=make_shared<Fog>(0.3,fogBoundary);
//	fog->material=make_shared<Isotropy>(vec3(0.576, 0.451, 0.647));
//	scene.objects.push_back(fog);

	Film image(camera.width, camera.height);

	const int antiAliasNum = 2;
	const int samples = 100;

	scene.constructBVH();


	//----------------------Render---------------------------
	PathTracer path;
	path.antiAliasNum = antiAliasNum;
	path.renderThreadNum = 4;
	path.samples = samples;
	path.renderPortionBlock = 8;//path.renderThreadNum;

	path.render(image, camera, scene);


	RTUI ui(800,600,"HJRT");
	ui.film=&image;
	ui.pathTracer=&path;
	ui.run();
	glfwTerminate();

	//----------------------Write image----------------------
	image.save("image.png", "png");
	cout << "finished" << endl;
	return 0;
}