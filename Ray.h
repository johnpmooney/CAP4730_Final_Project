#ifndef _RAY_H
#define _RAY_H

#include "Vector.h"

class Ray {
	Vector origin, direction;
	
	public:
	
	Ray(){
		origin = Vector(0,0,0);
		direction = Vector(1,0,0);
	}
	
	Ray(Vector a, Vector b){
		origin = a;
		direction = b;
	}
	
	Vector getRayOrigin(){
			return origin;
	}
	Vector getRayDirection(){
			return direction;
	}
	
};


#endif
