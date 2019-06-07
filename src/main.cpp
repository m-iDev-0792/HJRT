#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include "objects.h"
#include "mesh.h"
#include "pathtracer.h"

using namespace std;
using namespace glm;

int main() {
	Camera camera(vec3(0, 2, 14), vec3(0, 0, -1), 800, 600, 45);

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
//	white->albedoTex=make_shared<ImageTexture>("/Users/hezhenbang/Downloads/IMG_0450.JPG");

	auto floor1 = make_shared<Triangle>(vec3(-PlaneR, -2, PlaneR), vec3(PlaneR, -2, -PlaneR),
	                                    vec3(-PlaneR, -2, -PlaneR));
	floor1->name = "floor1";
	floor1->uv[0] = vec2(0, 0);
	floor1->uv[1] = vec2(1, 1);
	floor1->uv[2] = vec2(0, 1);
	auto floor2 = make_shared<Triangle>(vec3(-PlaneR, -2, PlaneR), vec3(PlaneR, -2, PlaneR), vec3(PlaneR, -2, -PlaneR));
	floor2->name = "floor2";
	floor2->uv[0] = vec2(0, 0);
	floor2->uv[1] = vec2(1, 0);
	floor2->uv[2] = vec2(1, 1);
	floor1->material = floor2->material = white;

	auto greenWall1 = make_shared<Triangle>(vec3(PlaneR, -2, -PlaneR), vec3(PlaneR, -2, PlaneR),
	                                        vec3(PlaneR, -2 + 2 * PlaneR, PlaneR));
	greenWall1->name = "greenWall1";
	auto greenWall2 = make_shared<Triangle>(vec3(PlaneR, -2, -PlaneR), vec3(PlaneR, -2 + 2 * PlaneR, PlaneR),
	                                        vec3(PlaneR, -2 + 2 * PlaneR, -PlaneR));
	greenWall2->name = "greenWall2";
	greenWall1->material = greenWall2->material = make_shared<Lambertian>(
			vec3(0.25, 0.75, 0.25));//(vec3(0.278, 0.494, 0.227));

	auto redWall1 = make_shared<Triangle>(vec3(-PlaneR, -2, PlaneR), vec3(-PlaneR, -2, -PlaneR),
	                                      vec3(-PlaneR, -2 + 2 * PlaneR, -PlaneR));
	redWall1->name = "redWall1";
	auto redWall2 = make_shared<Triangle>(vec3(-PlaneR, -2, PlaneR), vec3(-PlaneR, -2 + 2 * PlaneR, -PlaneR),
	                                      vec3(-PlaneR, -2 + 2 * PlaneR, PlaneR));
	redWall2->name = "redWall2";
	redWall1->material = redWall2->material = make_shared<Lambertian>(
			vec3(0.75, 0.25, 0.25));//(vec3(0.620, 0.173, 0.161));

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

	Scene scene;
	scene.useBVH = false;
	scene.ambient = vec3(0);
	scene.objects.push_back(sphere);
//	scene.objects.push_back(ground);
	scene.objects.push_back(metalSphere);
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

//	auto model=loadOBJ("tree02.obj");
//	scene.objects.insert(scene.objects.end(),model.begin(),model.end());

	Film image(camera.width, camera.height);

	const int antiAliasNum = 2;
	const int samples = 20;

	float subR = 1.0 / antiAliasNum;
	float subS = (-antiAliasNum / 2 + 0.5) * subR;

	scene.constructBVH();

	//----------------------Render---------------------------
	PathTracer path;
	path.antiAliasNum = antiAliasNum;
	path.renderThreads = 4;
	path.samples = samples;
	path.renderPortionBlock = path.renderThreads;
	path.render(image, camera, scene);

	//----------------------Write image----------------------
	image.save("image.ppm", "ppm");
	cout << "finished" << endl;
	return 0;
}