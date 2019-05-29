//
// Created by 何振邦 on 2019-05-28.
//

#include "bvh.h"
bool compX(const Object* a,const Object* b){
	AABB abox,bbox;
	if((!a->getAABB(&abox))||(!b->getAABB(&bbox))){
		cout<<"unable get AABB"<<endl;
		return false;
	}
	return abox.min.x<bbox.min.x;
}
bool compY(const Object* a,const Object* b){
	AABB abox,bbox;
	if((!a->getAABB(&abox))||(!b->getAABB(&bbox))){
		cout<<"unable get AABB"<<endl;
		return false;
	}
	return abox.min.y<bbox.min.y;
}
bool compZ(const Object* a,const Object* b){
	AABB abox,bbox;
	if((!a->getAABB(&abox))||(!b->getAABB(&bbox))){
		cout<<"unable get AABB"<<endl;
		return false;
	}
	return abox.min.z<bbox.min.z;
}

decltype(&compX) compList[]={&compX,&compY,&compZ};
BVH::BVH(vector<Object*>::iterator begin,vector<Object*>::iterator end){
	if(begin==end){
		leftNode=rightNode= nullptr;
		aabb=AABB(vec3(1),vec3(-1));
		return;
	}else if(end-begin==1){
		rightNode= nullptr;
		leftNode=*begin;
		if(!leftNode->getAABB(&aabb)){
			aabb=AABB(vec3(1),vec3(-1));
			cout<<"unable get AABB"<<endl;
		}
	}else if(end-begin==2){
		leftNode=*begin;
		rightNode=*(begin+1);
		AABB box1,box2;
		if((!leftNode->getAABB(&box1))||(!rightNode->getAABB(&box2))){
			aabb=AABB(vec3(1),vec3(-1));
			cout<<"unable get AABB"<<endl;
		}
		aabb=AABB::getBoundingAABB(box1,box2);
	}else{

	int initAxis=3*drand48();
	//sort objects
	sort(begin,end,compList[initAxis]);
	auto middle=begin+(end-begin)/2;

	leftNode=middle-begin>1?new BVH(begin,middle):*begin;
	rightNode=new BVH(middle,end);
		AABB boxLeft,boxRight;
		if((!leftNode->getAABB(&boxLeft))||(!rightNode->getAABB(&boxRight))){
			aabb=AABB(vec3(1),vec3(-1));
			cout<<"unable get AABB"<<endl;
		}
		aabb=AABB::getBoundingAABB(boxLeft,boxRight);
	}

}
bool BVH::intersect(const Ray &ray, HitInfo *hitInfo)const{
	if(aabb.hit(ray)){
		HitInfo leftInfo,rightInfo;
		bool leftHit=leftNode== nullptr?false:leftNode->intersect(ray,&leftInfo);
		bool rightHit=rightNode==nullptr?false:rightNode->intersect(ray,&rightInfo);
		if(leftHit&&rightHit){
			*hitInfo=leftInfo.t<rightInfo.t?leftInfo:rightInfo;
			return true;
		}else if(leftHit){
			*hitInfo=leftInfo;
			return true;
		}else if(rightHit){
			*hitInfo=rightInfo;
			return true;
		}
	}return false;
}
bool BVH::getAABB(AABB* box)const{
	*box=aabb;
	return true;
}