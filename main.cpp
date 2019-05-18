#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>
#include "Objects.h"
#include "Scene.h"
#include "Camera.h"
using namespace std;
using namespace glm;

int main() {

	Camera camera(vec3(0,0,8),vec3(0,0,-1),800,600,1,45);

	Sphere sphere(vec3(0,-1,0),1);
	sphere.material=make_shared<Lambertian>(vec3(230,173,104)*(1/255.0f));

	Sphere metalSphere(vec3(2,-1,-2),1);
	metalSphere.material=make_shared<Metal>(vec3(0.8),0.5);

	Sphere ground(vec3(0,-2-100,0),100);
	ground.material=make_shared<Lambertian>(vec3(189,211,223)*(1/255.0f));

	Sphere light(vec3(3,3,0),1);
	light.material=make_shared<Lambertian>(vec3(0));
	light.material->emission=vec3(5);

	const int PlaneR=4;
	Triangle triangle1(vec3(-PlaneR,-2,PlaneR),vec3(PlaneR,-2,-PlaneR),vec3(-PlaneR,-2,-PlaneR));
	Triangle triangle2(vec3(-PlaneR,-2,PlaneR),vec3(PlaneR,-2,PlaneR),vec3(PlaneR,-2,-PlaneR));
	Scene scene;
	scene.objects.push_back(&sphere);
	scene.objects.push_back(&ground);
	scene.objects.push_back(&light);
	scene.objects.push_back(&metalSphere);
//	scene.objects.push_back(&triangle1);
//	scene.objects.push_back(&triangle2);

	vec3 *data=new vec3[camera.width*camera.height];

	const int antiAliasNum=2;
	const int samples=120;

	vec3 color(0);

	float subR=1.0/antiAliasNum;
	float subS=(-antiAliasNum/2+0.5)*subR;

	//----------------------Render----------------------
#pragma omp parallel for schedule(dynamic, 1) private(color)
	for(int i=0;i<camera.height;++i){
		cout<<"progress = "<<(i*100.0f/camera.height)<<"%"<<endl;
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
			data[i*camera.width+j]=color;
		}
	}

	//----------------------Write image----------------------
	cout<<"finished"<<endl;
	savePPM(data,camera.width,camera.height,"image.ppm");
	delete [] data;
	return 0;
}