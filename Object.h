#ifndef _OBJECT_H
#define _OBJECT_H

#include "Vector.h"
#include "Color.h"
#include "Ray.h"

class Object {
	
	public:
	
	Object(){};
	
	virtual Color getColor() {
		return Color (0.0,0.0,0.0,0.0);
	}
	
	virtual Vector getNormal(Vector position) {
		return Vector (0,0,0);
	}
		
	virtual double intersection(Ray ray) {
		return 0;
	}
	
	virtual Vector getSphereCenter() {
		return Vector (0,0,0);
	}
	
	
	
};

#endif
