//
// Created by 何振邦 on 2019-08-09.
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
	Camera camera(vec3(0, 10, 19), vec3(0, 5, 0), 512 / 2 + 128, 512 / 2, 45);

	auto sphere = make_shared<Sphere>(vec3(0, 0, 0), 2);
	sphere->name = "diffuse";
	sphere->material = make_shared<Lambertian>(vec3(230, 173, 104) * (1 / 255.0f));

	auto metalSphere = make_shared<Sphere>(vec3(0, 0, 0), 2);
	metalSphere->name = "metal";
	metalSphere->material = make_shared<Metal>(vec3(0.8), 0.3);

	auto glassSphere = make_shared<Sphere>(vec3(-2, 1, 0), 1);
	glassSphere->name = "glass";
	glassSphere->material = make_shared<Dielectric>(vec3(1), 1.5);

	auto light = make_shared<Sphere>(vec3(0, 5, 0), 1);
	light->name = "light";
	light->material = make_shared<Lambertian>(vec3(0), vec3(20));
	light->material->emitDirection = vec3(0);

	const int PlaneR = 4;
	auto white = make_shared<Lambertian>(vec3(0.73));

	//Floor
	auto floor = make_shared<Plane>(vec3(-PlaneR, -2, PlaneR),
	                                vec3(PlaneR, -2, PlaneR),
	                                vec3(PlaneR, -2, -PlaneR),
	                                vec3(-PlaneR, -2, -PlaneR));
	floor->setMaterial(white);

	//Green Wall
	auto greenWall = make_shared<Plane>(vec3(PlaneR, -2, -PlaneR),
	                                    vec3(PlaneR, -2, PlaneR),
	                                    vec3(PlaneR, -2 + 2 * PlaneR, PlaneR),
	                                    vec3(PlaneR, -2 + 2 * PlaneR, -PlaneR));
	greenWall->setMaterial(make_shared<Lambertian>(vec3(0.12, 0.45, 0.15)));

	//Red Wall
	auto redWall = make_shared<Plane>(vec3(-PlaneR, -2, PlaneR),
	                                  vec3(-PlaneR, -2, -PlaneR),
	                                  vec3(-PlaneR, -2 + 2 * PlaneR, -PlaneR),
	                                  vec3(-PlaneR, -2 + 2 * PlaneR, PlaneR));
	redWall->setMaterial(make_shared<Lambertian>(vec3(0.65, 0.05, 0.05)));

	//Ceil
	auto ceil = make_shared<Plane>(vec3(-PlaneR, -2 + 2 * PlaneR, -PlaneR),
	                               vec3(PlaneR, -2 + 2 * PlaneR, -PlaneR),
	                               vec3(PlaneR, -2 + 2 * PlaneR, PlaneR),
	                               vec3(-PlaneR, -2 + 2 * PlaneR, PlaneR));
	ceil->setMaterial(white);

	//Back
	auto back = make_shared<Plane>(vec3(-PlaneR, -2, -PlaneR),
	                               vec3(PlaneR, -2, -PlaneR),
	                               vec3(PlaneR, -2 + 2 * PlaneR, -PlaneR),
	                               vec3(-PlaneR, -2 + 2 * PlaneR, -PlaneR));
	back->setMaterial(white);

	//Ceil Light
	auto rectangleLight = make_shared<Plane>(vec3(-PlaneR / 4, -2.05 + 2 * PlaneR, -PlaneR / 4),
	                                         vec3(PlaneR / 4, -2.05 + 2 * PlaneR, -PlaneR / 4),
	                                         vec3(PlaneR / 4, -2.05 + 2 * PlaneR, PlaneR / 4),
	                                         vec3(-PlaneR / 4, -2.05 + 2 * PlaneR, PlaneR / 4));
	auto ceilLightMaterial = make_shared<Lambertian>(vec3(0), vec3(20));
	ceilLightMaterial->emitDirection = vec3(0, -1, 0);
	rectangleLight->setMaterial(ceilLightMaterial);

	mat4 enlarge(1.0f);
	enlarge = scale(enlarge, vec3(1.5, 1, 1.5));


	Scene scene;
	scene.useBVH = false;//turn on when there are many objects in the scene
	scene.ambient = vec3(0);
	scene.shutterPeriod = TimePeriod(0, 1.0);
//	scene.addShape(sphere);
//	scene.addShape(glassSphere);

//	scene.addShape(rectangleLight);
//	scene.addShape(floor);
//	scene.addShape(ceil);
//	scene.addShape(redWall);
//	scene.addShape(greenWall);
//	scene.addShape(back);

//	auto mesh = make_shared<Mesh>();
//	mesh->material = make_shared<Dielectric>(vec3(1), 2.417);
//	glm::mat4 meshTransMat(1.0f);
//	meshTransMat = translate(meshTransMat, vec3(0, -2, 0));
//	mesh->loadMesh("../mesh/diamondStanding.obj");
//	mesh->transform(meshTransMat);
//	cout << "loaded model: " << mesh->name << "  triangle num: " << mesh->triangles.size() << endl;
//	scene.addShape(mesh);

	auto mesh = make_shared<Mesh>();
	glm::mat4 meshTransMat(1.0f);
	meshTransMat = translate(meshTransMat, vec3(0, -2, 0));
	mesh->loadMesh("background.obj");
//	mesh->transform(meshTransMat);
	cout << "loaded model: " << mesh->name << "  triangle num: " << mesh->triangles.size() << endl;
	scene.addShape(mesh);

	auto sbinner=make_shared<Mesh>();
	sbinner->setMaterial(make_shared<Lambertian>(vec3(0.6)));
	sbinner->loadMesh("shaderball-inner.obj");
	scene.addShape(sbinner);

	auto sboutside=make_shared<Mesh>();
	sboutside->setMaterial(make_shared<Dielectric>(vec3(1),1.4,0.8,0.8));
	sboutside->loadMesh("shaderball-outside.obj");
	scene.addShape(sboutside);

	mat4 smaller(1.0);
	smaller=scale(smaller,vec3(1.0f/50));
	sbinner->transform(smaller);
	sboutside->transform(smaller);
	///////////////////////////////////
	auto sbinner2=make_shared<Mesh>();
	sbinner2->setMaterial(make_shared<Lambertian>(vec3(0.6)));
	sbinner2->loadMesh("shaderball-inner.obj");
	scene.addShape(sbinner2);

	auto sboutside2=make_shared<Mesh>();
	sboutside2->setMaterial(make_shared<Metal>(vec3(0.8)));
	sboutside2->loadMesh("shaderball-outside.obj");
	scene.addShape(sboutside2);

	mat4 smaller2(1.0);
	smaller2=translate(smaller2,vec3(-8,0,0));
	smaller2=scale(smaller2,vec3(1.0f/50));
	sbinner2->transform(smaller2);
	sboutside2->transform(smaller2);

	//////////////////////////////////
	auto sbinner3=make_shared<Mesh>();
	sbinner3->setMaterial(make_shared<Lambertian>(vec3(0.6)));
	sbinner3->loadMesh("shaderball-inner.obj");
	scene.addShape(sbinner3);

	auto sboutside3=make_shared<Mesh>();
	sboutside3->setMaterial(make_shared<Lambertian>(vec3(0.639, 0.416, 0.800)));
	sboutside3->loadMesh("shaderball-outside.obj");
	scene.addShape(sboutside3);

	mat4 smaller3(1.0);
	smaller3=translate(smaller3,vec3(8,0,0));
	smaller3=scale(smaller3,vec3(1.0f/50));
	sbinner3->transform(smaller3);
	sboutside3->transform(smaller3);


	auto testlight = make_shared<Sphere>(vec3(0, 6 * PlaneR, 0), 5);
	testlight->name = "testlight";
	testlight->material = make_shared<Lambertian>(vec3(0), vec3(10));
	testlight->material->emitDirection = vec3(0);
	scene.addShape(testlight);

	const int antiAliasNum = 2;
	const int samples = 1;

	//!Multiple Importance Sampling!
	auto objectSampler = make_shared<ObjectSampler>(testlight);
	auto cosineSampler = make_shared<CosineHemisphereSampler>();
	auto mixSampler = make_shared<MixtureSampler>(objectSampler, cosineSampler, 0.5);
	for (int i = 0; i < scene.objects.size(); ++i)scene.objects[i]->setSampler(mixSampler);
	//!Multiple Importance Sampling!


	//----------------------Render---------------------------
	PathTracer path;
	path.antiAliasNum = antiAliasNum;
	path.renderThreadNum = 4;
	path.samples = samples;
	path.renderPortionBlock = 16;//path.renderThreadNum;

	path.render(camera, scene);


	RTUI ui(800, 600, "HJRT");
	ui.film = &camera.film;
	ui.pathTracer = &path;
	ui.run();
	cout << "rendering time:" << secondToFormatTime(path.latestRenderSec) << endl;
	glfwTerminate();

	//----------------------Write image----------------------
	camera.film.save("image.png");
	cout << "finished" << endl;
	return 0;
}