#ifndef _PLANE_H
#define _PLANE_H

#include "Vector.h"
#include "Color.h"
#include "math.h"
#include "Object.h"

class Plane : public Object {
	Vector normal;
	double distance;
	Color color;
	
	public:
	
	Plane(){
		normal = Vector(1,0,0);
		distance = 0;
		color = Color(0.5, 0.5, 0.5, 0);
	}
	
	Plane(Vector a, double b, Color c){
		normal = a;
		distance = b;
		color = c;
	}

	
	Vector getPlaneNormal(){
			return normal;
	}
    double getPlaneDistance(){
			return distance;
	}
	virtual Color getColor(){
			return color;
	}
	
	virtual Vector getNormal(Vector point) {
		return normal;
	}
	
	virtual double intersection(Ray ray) {
		Vector rayDirection = ray.getRayDirection();
		double m = rayDirection.dotProduct(normal);
		if(m == 0){//parallel to plane
			return -1;
		}
		else {
			double n = normal.dotProduct(ray.getRayOrigin().vectorAddition(normal.vectorScalar(distance).invert()));
			return -1*n/m;
		}
			
	}
	
};

#endif
