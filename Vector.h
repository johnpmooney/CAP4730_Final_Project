#ifndef _VECTOR_H
#define _VECTOR_H

#include "math.h"


class Vector {
	double x,y,z;
	
	public:
	
	Vector(){
		x=0;
		y=0;
		z=0;
	}
	
	Vector(double a, double b, double c){
		x=a;
		y=b;
		z=c;
	}
	
	double getVectorX(){
			return x;
	}
	double getVectorY(){
			return y;
	}
	double getVectorZ(){
			return z;
	}
	
	double magnitude(){
		return sqrt((x*x)+(y*y)+(z*z));
	}
	
	Vector normalize(){
		double magnitude = sqrt((x*x)+(y*y)+(z*z));
		return Vector(x/magnitude,y/magnitude,z/magnitude);
	}
	
	Vector invert(){
		return Vector(-x,-y,-z);
	}
	
	double dotProduct(Vector v){
		return (x*v.getVectorX() + y*v.getVectorY() + z*v.getVectorZ());
	}
	
	Vector crossProduct(Vector v){
		return Vector(y*v.getVectorZ() - z*v.getVectorY(),
						z*v.getVectorX() - x*v.getVectorZ(),
						x*v.getVectorY() - y*v.getVectorX());
	}
	
	Vector vectorAddition(Vector v){
		return Vector(x + v.getVectorX(), y + v.getVectorY(), z + v.getVectorZ());
	}
	
	Vector vectorScalar(double scalar){
		return Vector(x*scalar,y*scalar,z*scalar);
	}
						
};


#endif
