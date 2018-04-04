#ifndef _CAMERA_H
#define _CAMERA_H

#include "Vector.h"


class  Camera{
	Vector cameraPosition, cameraDirection, cameraRight, cameraDown;
	
	public:
	
	Camera(){
	cameraPosition = Vector(0,0,0);
	cameraDirection = Vector(0,0,1);
	cameraRight = Vector(0,0,0);
	cameraDown = Vector(0,0,0);
	}
	
	Camera(Vector position,Vector direction,Vector right,Vector down){
	cameraPosition = position;
	cameraDirection = direction;
	cameraRight = right;
	cameraDown = down;
	}
	
	Vector getCameraPosition(){
			return cameraPosition;
	}
	Vector getCameraDirection(){
			return cameraDirection;
	}
	Vector getCameraRight(){
			return cameraRight;
	}
	Vector getCameraDown(){
			return cameraDown;
	}
	
};

#endif
