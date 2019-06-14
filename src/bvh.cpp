//
// Created by 何振邦 on 2019-05-28.
//

#include "bvh.h"
bool compX(const std::shared_ptr<Object> a,const std::shared_ptr<Object> b){
	AABB abox,bbox;
	if((!a->getAABB(&abox))||(!b->getAABB(&bbox))){
		std::cout<<"unable get AABB"<<std::endl;
		return false;
	}
	return abox.min.x<bbox.min.x;
}
bool compY(const std::shared_ptr<Object> a,const std::shared_ptr<Object> b){
	AABB abox,bbox;
	if((!a->getAABB(&abox))||(!b->getAABB(&bbox))){
		std::cout<<"unable get AABB"<<std::endl;
		return false;
	}
	return abox.min.y<bbox.min.y;
}
bool compZ(const std::shared_ptr<Object> a,const std::shared_ptr<Object> b){
	AABB abox,bbox;
	if((!a->getAABB(&abox))||(!b->getAABB(&bbox))){
		std::cout<<"unable get AABB"<<std::endl;
		return false;
	}
	return abox.min.z<bbox.min.z;
}

decltype(&compX) compList[]={&compX,&compY,&compZ};
BVH::BVH(std::vector<std::shared_ptr<Object>>::iterator begin,std::vector<std::shared_ptr<Object>>::iterator end){
	if(begin==end){
		leftNode=rightNode= nullptr;
		aabb=AABB(glm::vec3(1),glm::vec3(-1));
		return;
	}else if(end-begin==1){
		rightNode= nullptr;
		leftNode=*begin;
		if(!leftNode->getAABB(&aabb)){
			aabb=AABB(glm::vec3(1),glm::vec3(-1));
			std::cout<<"unable get AABB"<<std::endl;
		}
	}else if(end-begin==2){
		leftNode=*begin;
		rightNode=*(begin+1);
		AABB box1,box2;
		if((!leftNode->getAABB(&box1))||(!rightNode->getAABB(&box2))){
			aabb=AABB(glm::vec3(1),glm::vec3(-1));
			std::cout<<"unable get AABB"<<std::endl;
		}
		aabb=AABB::getBoundingAABB(box1,box2);
	}else{

	int initAxis=3*random0_1f();
	//sort objects
	std::sort(begin,end,compList[initAxis]);
	auto middle=begin+(end-begin)/2;

	leftNode=middle-begin>1?std::make_shared<BVH>(begin,middle):*begin;
	rightNode=std::make_shared<BVH>(middle,end);
		AABB boxLeft,boxRight;
		if((!leftNode->getAABB(&boxLeft))||(!rightNode->getAABB(&boxRight))){
			aabb=AABB(glm::vec3(1),glm::vec3(-1));
			std::cout<<"unable get AABB"<<std::endl;
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