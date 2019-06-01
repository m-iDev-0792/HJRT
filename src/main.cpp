#include <iostream>
#include <fstream>
#include <vector>
//#include <omp.h>
#include <chrono>
#include "objects.h"
#include "scene.h"
#include "camera.h"
#include "imageExporter.h"
using namespace std;
using namespace glm;

int main() {

	Camera camera(vec3(0,2,14),vec3(0,0,-1),800,600,45);

	Sphere sphere(vec3(0,-1,0),1);
	sphere.name="diffuse";
	sphere.material=make_shared<Lambertian>(vec3(230,173,104)*(1/255.0f));

	Sphere metalSphere(vec3(2,-1,-2),1);
	metalSphere.name="metal";
	metalSphere.material=make_shared<Metal>(vec3(0.8),0.5);

	Sphere glassSphere(vec3(2,-1,0),1);
	glassSphere.name="glass";
	glassSphere.material=make_shared<Dielectric>(1.5);

	Sphere ground(vec3(0,-2-100,0),100);
	ground.name="ground";
	ground.material=make_shared<Lambertian>(vec3(189,211,223)*(1/255.0f));

	Sphere light(vec3(0,6,0),1);
	light.name="light";
	light.material=make_shared<Lambertian>(vec3(0));
	light.material->emission=vec3(10);

	const int PlaneR=4;
	auto white=make_shared<Lambertian>(vec3(0.8));
	white->albedoTex=make_shared<ImageTexture>("/Users/hezhenbang/Downloads/IMG_0450.JPG");
	Triangle floor1(vec3(-PlaneR,-2,PlaneR),vec3(PlaneR,-2,-PlaneR),vec3(-PlaneR,-2,-PlaneR));
	floor1.uv[0]=vec2(0,0);floor1.uv[1]=vec2(1,1);floor1.uv[2]=vec2(0,1);
	Triangle floor2(vec3(-PlaneR,-2,PlaneR),vec3(PlaneR,-2,PlaneR),vec3(PlaneR,-2,-PlaneR));
	floor2.uv[0]=vec2(0,0);floor2.uv[1]=vec2(1,0);floor2.uv[2]=vec2(1,1);
	floor1.material=floor2.material=white;

	Triangle greenWall1(vec3(PlaneR,-2,-PlaneR),vec3(PlaneR,-2,PlaneR),vec3(PlaneR,-2+2*PlaneR,PlaneR));
	Triangle greenWall2(vec3(PlaneR,-2,-PlaneR),vec3(PlaneR,-2+2*PlaneR,PlaneR),vec3(PlaneR,-2+2*PlaneR,-PlaneR));
	greenWall1.material=greenWall2.material=make_shared<Lambertian>(vec3(0.25, 0.75, 0.25));//(vec3(0.278, 0.494, 0.227));

	Triangle redWall1(vec3(-PlaneR,-2,PlaneR),vec3(-PlaneR,-2,-PlaneR),vec3(-PlaneR,-2+2*PlaneR,-PlaneR));
	Triangle redWall2(vec3(-PlaneR,-2,PlaneR),vec3(-PlaneR,-2+2*PlaneR,-PlaneR),vec3(-PlaneR,-2+2*PlaneR,PlaneR));
	redWall1.material=redWall2.material=make_shared<Lambertian>(vec3(0.75, 0.25, 0.25));//(vec3(0.620, 0.173, 0.161));

	Triangle ceil1(vec3(-PlaneR,-2+2*PlaneR,PlaneR),vec3(PlaneR,-2+2*PlaneR,-PlaneR),vec3(-PlaneR,-2+2*PlaneR,-PlaneR));
	Triangle ceil2(vec3(-PlaneR,-2+2*PlaneR,PlaneR),vec3(PlaneR,-2+2*PlaneR,PlaneR),vec3(PlaneR,-2+2*PlaneR,-PlaneR));
	ceil1.material=ceil2.material=white;

	Triangle back1(vec3(-PlaneR,-2,-PlaneR),vec3(PlaneR,-2,-PlaneR),vec3(PlaneR,-2+2*PlaneR,-PlaneR));
	Triangle back2(vec3(-PlaneR,-2,-PlaneR),vec3(PlaneR,-2+2*PlaneR,-PlaneR),vec3(-PlaneR,-2+2*PlaneR,-PlaneR));
	back1.material=back2.material=white;

	Triangle light1(vec3(-PlaneR/4,-2.05+2*PlaneR,PlaneR/4),vec3(PlaneR/4,-2.05+2*PlaneR,-PlaneR/4),vec3(-PlaneR/4,-2.05+2*PlaneR,-PlaneR/4));
	Triangle light2(vec3(-PlaneR/4,-2.05+2*PlaneR,PlaneR/4),vec3(PlaneR/4,-2.05+2*PlaneR,PlaneR/4),vec3(PlaneR/4,-2.05+2*PlaneR,-PlaneR/4));
	light1.material=light2.material=light.material;

	Scene scene;
	scene.useBVH=false;
	scene.ambient=vec3(0);
	scene.objects.push_back(&sphere);
	scene.objects.push_back(&ground);
	scene.objects.push_back(&metalSphere);
	scene.objects.push_back(&light1);
	scene.objects.push_back(&light2);

//	scene.objects.push_back(&floor1);
//	scene.objects.push_back(&floor2);

//	scene.objects.push_back(&ceil1);
//	scene.objects.push_back(&ceil2);
//
//	scene.objects.push_back(&redWall1);
//	scene.objects.push_back(&redWall2);
//
//	scene.objects.push_back(&greenWall1);
//	scene.objects.push_back(&greenWall2);
//
//	scene.objects.push_back(&back1);
//	scene.objects.push_back(&back2);


	ImageExporter image(camera.width,camera.height);

	const int antiAliasNum=2;
	const int samples=10;

	vec3 color(0);

	float subR=1.0/antiAliasNum;
	float subS=(-antiAliasNum/2+0.5)*subR;

	//----------------------Render----------------------
	scene.constructBVH();
//	scene.shade(camera.getRay(400+subS+subR*0,120+subS+subR*0),1);
//	return 0;
	auto start=chrono::high_resolution_clock::now();
#pragma omp parallel for schedule(dynamic, 1) private(color)
	for(int i=0;i<camera.height;++i){
		if(!(i%5))cout<<"progress = "<<(i*100.0f/camera.height)<<"%"<<endl;
		for(int j=0;j<camera.width;++j){

			color=vec3(0);
			for(int u=0;u<antiAliasNum;++u){
				for(int v=0;v<antiAliasNum;++v){

					for(int s=0;s<samples;++s)
						color=color+scene.shade(camera.getRay(j+subS+subR*u,i+subS+subR*v),1);

				}
			}
			color=color*(1.0f/(antiAliasNum*antiAliasNum*samples));
			gammaCorrection(color);
			color*=255;
			toInt(color);
			image.setPixel(i,j,color);
		}
	}
	auto end=chrono::high_resolution_clock::now();
	chrono::high_resolution_clock::duration dura=end-start;
	auto secDura=chrono::duration_cast<chrono::seconds>(dura);
	cout<<"total rendering time: "<<secDura.count()<<"s"<<endl;
	//----------------------Write image----------------------
	image.save("image.ppm","ppm");
	cout<<"finished"<<endl;
	return 0;
}