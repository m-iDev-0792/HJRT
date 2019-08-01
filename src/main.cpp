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

	auto sphere = make_shared<Sphere>(vec3(0, -1, 0), 1);
	sphere->name = "diffuse";
	sphere->material = make_shared<Lambertian>(vec3(230, 173, 104) * (1 / 255.0f));

	auto metalSphere = make_shared<Sphere>(vec3(2, -1, -2), 1);
	metalSphere->name = "metal";
	metalSphere->material = make_shared<Metal>(vec3(0.8), 0);

	auto glassSphere = make_shared<Sphere>(vec3(2, 0, 0), 1);
	glassSphere->name = "glass";
	glassSphere->material = make_shared<Dielectric>(1.5);

	auto ground = make_shared<Sphere>(vec3(0, -2 - 100, 0), 100);
	ground->name = "ground";
	ground->material = make_shared<Lambertian>(vec3(189, 211, 223) * (1 / 255.0f));

	auto light = make_shared<Sphere>(vec3(0, 6, 0), 1);
	light->name = "light";
	light->material = make_shared<Lambertian>(vec3(0));
	light->material->emission = vec3(20);
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
	greenWall->setMaterial(make_shared<Lambertian>(make_shared<SolidColorTexture>(0.12, 0.45, 0.15)));

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
//	scene.objects.push_back(sphere);
//	scene.objects.push_back(ground);
//	scene.objects.push_back(metalSphere);

	scene.objects.push_back(rectangleLight);
	scene.objects.push_back(floor);
	scene.objects.push_back(ceil);
	scene.objects.push_back(redWall);
	scene.objects.push_back(greenWall);
	scene.objects.push_back(back);

	auto movingSphere=make_shared<MovingSphere>(vec3(0),1,vec3(0,1,0));
	movingSphere->material=metalSphere->material;
	movingSphere->name="movingSphere";
//	scene.objects.push_back(movingSphere);


//	auto model=make_shared<Mesh>();
//	model->transMat=scale(model->transMat,vec3(1.5f));
//	model->transMat=translate(model->transMat,vec3(0,0.35,0));
//	model->material=glassSphere->material;
//	model->loadMesh("../mesh/bunny.obj");
//	cout<<"loaded model: "<<model->name<<"  triangle num: "<<model->triangles.size()<<endl;
//	scene.objects.push_back(model);

	auto mesh = make_shared<Mesh>();
	mesh->material = make_shared<Dielectric>(2.417);
	mesh->transMat = translate(mesh->transMat, vec3(0, -2, 0));
	mesh->loadMesh("../mesh/diamondStanding.obj");
	cout << "loaded model: " << mesh->name << "  triangle num: " << mesh->triangles.size() << endl;
	scene.objects.push_back(mesh);

/*	auto mesh = make_shared<Mesh>();
	mesh->material = metalSphere->material;//make_shared<Dielectric>(2.417);
	mesh->transMat = translate(mesh->transMat, vec3(0, -2, 0));
	mesh->transMat = rotate(mesh->transMat, radians(150.0f),vec3(0,-1,0));
	mesh->transMat = scale(mesh->transMat, vec3(4));
	mesh->loadMesh("../mesh/dragon.obj");
	cout << "loaded model: " << mesh->name << "  triangle num: " << mesh->triangles.size() << endl;
	scene.objects.push_back(mesh);*/



//-----fog example-----
//	auto fogBoundary=make_shared<Sphere>(vec3(0,0,0),2);
//	auto fog=make_shared<Fog>(0.3,fogBoundary);
//	fog->material=make_shared<Isotropy>(vec3(0.576, 0.451, 0.647));
//	scene.objects.push_back(fog);

	Film image(camera.width, camera.height);

	const int antiAliasNum = 2;
	const int samples = 20;

	//!Multiple Importance Sampling!
	auto objectSampler = make_shared<ObjectSampler>(rectangleLight);
	auto cosineSampler = make_shared<CosineHemisphereSampler>();
	auto mixSampler = make_shared<MixtureSampler>(objectSampler, cosineSampler, 0.3);
	for (int i = 0; i < scene.objects.size(); ++i)scene.objects[i]->material->sampler = mixSampler;
	//!Multiple Importance Sampling!

//	scene.constructBVH();

	//----------------------Render---------------------------
	PathTracer path;
	path.antiAliasNum = antiAliasNum;
	path.renderThreadNum = 4;
	path.samples = samples;
	path.renderPortionBlock = 8;//path.renderThreadNum;

	path.render(image, camera, scene,TimePeriod(0,1.0));


	RTUI ui(800, 600, "HJRT");
	ui.film = &image;
	ui.pathTracer = &path;
	ui.run();
	glfwTerminate();

	//----------------------Write image----------------------
	image.save("image.exr", "exr");
	cout << "finished" << endl;
	return 0;
}