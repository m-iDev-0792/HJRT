### Diamond
```cpp
	auto mesh = make_shared<Mesh>();
	mesh->material = make_shared<Dielectric>(vec3(1), 2.417);
	glm::mat4 meshTransMat(1.0f);
	meshTransMat = translate(meshTransMat, vec3(0, -2, 0));
	mesh->loadMesh("../mesh/diamondStanding.obj");
	mesh->transform(meshTransMat);
	cout << "loaded model: " << mesh->name << "  triangle num: " << mesh->triangles.size() << endl;
	scene.addShape(mesh);
```

### Toasters
```cpp
	auto mesh = make_shared<Mesh>();
	glm::mat4 meshTransMat(1.0f);
	meshTransMat = scale(meshTransMat, vec3(0.008));
	mesh->loadMesh("Toasters.obj");
	mesh->transform(meshTransMat);
	cout << "loaded model: " << mesh->name << "  triangle num: " << mesh->triangles.size() << endl;
	scene.addShape(mesh);
```

### Bunny
```cpp
	auto model=make_shared<Mesh>();
	model->transMat=scale(model->transMat,vec3(1.5f));
	model->transMat=translate(model->transMat,vec3(0,0.35,0));
	model->material=glassSphere->material;
	model->loadMesh("../mesh/bunny.obj");
	cout<<"loaded model: "<<model->name<<"  triangle num: "<<model->triangles.size()<<endl;
	scene.addShape(model);
```

### Dragon
```cpp
	auto mesh = make_shared<Mesh>();
	mesh->material = metalSphere->material;//make_shared<Dielectric>(2.417);
	mesh->transMat = translate(mesh->transMat, vec3(0, -2, 0));
	mesh->transMat = rotate(mesh->transMat, radians(150.0f),vec3(0,-1,0));
	mesh->transMat = scale(mesh->transMat, vec3(4));
	mesh->loadMesh("../mesh/dragon.obj");
	cout << "loaded model: " << mesh->name << "  triangle num: " << mesh->triangles.size() << endl;
	scene.addShape(mesh);
```

### Fog
```cpp
	auto fogBoundary=make_shared<Sphere>(vec3(0,0,0),2);
	auto fog=make_shared<Fog>(0.3,fogBoundary);
	fog->material=make_shared<Isotropy>(vec3(0.576, 0.451, 0.647));
	scene.addShape(fog);
```

### Motion Blur
```cpp
	auto movingSphere=make_shared<MovingSphere>(vec3(0),1,vec3(0,1,0));
	movingSphere->material=metalSphere->material;
	movingSphere->name="movingSphere";
	scene.addShape(movingSphere);
```