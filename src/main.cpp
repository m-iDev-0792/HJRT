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
	Camera camera(vec3(0, 2, 14), vec3(0, 0, -1), 512/2, 512/2, 45);

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
	auto floor = make_shared<Plane>(vec3(-PlaneR, -2,  PlaneR),
	                                vec3( PlaneR, -2,  PlaneR),
	                                vec3( PlaneR, -2, -PlaneR),
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
	                               vec3( PlaneR, -2 + 2 * PlaneR, -PlaneR),
	                               vec3( PlaneR, -2 + 2 * PlaneR,  PlaneR),
	                               vec3(-PlaneR, -2 + 2 * PlaneR,  PlaneR));
	ceil->setMaterial(white);

	//Back
	auto back = make_shared<Plane>(vec3(-PlaneR, -2, -PlaneR),
	                               vec3( PlaneR, -2, -PlaneR),
	                               vec3( PlaneR, -2 + 2 * PlaneR, -PlaneR),
	                               vec3(-PlaneR, -2 + 2 * PlaneR, -PlaneR));
	back->setMaterial(white);

	//Ceil Light
	auto rectangleLight = make_shared<Plane>(vec3(-PlaneR / 4, -2.05 + 2 * PlaneR, -PlaneR / 4),
	                                         vec3( PlaneR / 4, -2.05 + 2 * PlaneR, -PlaneR / 4),
	                                         vec3( PlaneR / 4, -2.05 + 2 * PlaneR,  PlaneR / 4),
	                                         vec3(-PlaneR / 4, -2.05 + 2 * PlaneR,  PlaneR / 4));
	auto ceilLightMaterial = make_shared<Lambertian>(vec3(0),vec3(20));
	ceilLightMaterial->emitDirection=vec3(0,-1,0);
	rectangleLight->setMaterial(ceilLightMaterial);

	mat4 enlarge(1.0f);
	enlarge = scale(enlarge, vec3(1.5, 1, 1.5));


	Scene scene;
	scene.useBVH = false;
	scene.ambient = vec3(0);
	scene.shutterPeriod=TimePeriod(0,1.0);
//	scene.addShape(sphere);
//	scene.addShape(glassSphere);

	scene.addShape(rectangleLight);
	scene.addShape(floor);
	scene.addShape(ceil);
	scene.addShape(redWall);
	scene.addShape(greenWall);
	scene.addShape(back);

	auto mesh = make_shared<Mesh>();
	mesh->material = make_shared<Dielectric>(vec3(1), 2.417);
	glm::mat4 meshTransMat(1.0f);
	meshTransMat = translate(meshTransMat, vec3(0, -2, 0));
	mesh->loadMesh("../mesh/diamondStanding.obj");
	mesh->transform(meshTransMat);
	cout << "loaded model: " << mesh->name << "  triangle num: " << mesh->triangles.size() << endl;
	scene.addShape(mesh);


	Film image(camera.width, camera.height);

	const int antiAliasNum = 2;
	const int samples = 20;

	//!Multiple Importance Sampling!
	auto objectSampler = make_shared<ObjectSampler>(rectangleLight);
	auto cosineSampler = make_shared<CosineHemisphereSampler>();
	auto mixSampler = make_shared<MixtureSampler>(objectSampler, cosineSampler, 0.5);
	for (int i = 0; i < scene.objects.size(); ++i)scene.objects[i]->setSampler(mixSampler);
	//!Multiple Importance Sampling!


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
	cout<<"rendering time:"<<path.latestRenderSec<<"s"<<endl;
	glfwTerminate();

	//----------------------Write image----------------------
	image.save("image.png", "png");
	cout << "finished" << endl;
	return 0;
}