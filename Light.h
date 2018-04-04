#ifndef _LIGHT_H
#define _LIGHT_H

#include "Vector.h"
#include "Color.h"
#include "Source.h"

class Light : public Source {
	Vector position;
	Color color;
	
	public:
	
	Light(){
		position = Vector(0,0,0);
		color = Color(1,1,1,0);
	}
	
	Light(Vector a, Color b){
		position = a;
		color = b;
	}
	
	virtual Vector getLightPosition(){
			return position;
	}
	virtual Color getLightColor(){
			return color;
	}
	
};


#endif