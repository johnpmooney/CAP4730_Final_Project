#ifndef _SPHERE_H
#define _SPHERE_H

#include "Vector.h"
#include "Color.h"
#include "math.h"
#include "Object.h"

class Sphere : public Object {
	Vector center;
	double radius;
	Color color;
	
	public:
	
	Sphere(){
		center = Vector(0,0,0);
		radius = 1.0;
		color = Color(0.5, 0.5, 0.5, 0);
	}
	
	Sphere(Vector a, double b, Color c){
		center = a;
		radius = b;
		color = c;
	}
	
	virtual Vector getSphereCenter(){
			return center;
	}
    double getSphereRadius(){
			return radius;
	}
	virtual Color getColor(){
			return color;
	}
	virtual Vector getNormal(Vector point) {
		Vector normal = point.vectorAddition(center.invert()).normalize();
		return normal;
	}
	
	virtual double intersection(Ray ray) {
		Vector rayOrigin = ray.getRayOrigin();
		double rayOriginX = rayOrigin.getVectorX();
		double rayOriginY = rayOrigin.getVectorY();
		double rayOriginZ = rayOrigin.getVectorZ();
		
		Vector rayDirection = ray.getRayDirection();
		double rayDirectionX = rayDirection.getVectorX();
		double rayDirectionY = rayDirection.getVectorY();
		double rayDirectionZ = rayDirection.getVectorZ();
		
		Vector sphereCenter = center;
		double sphereCenterX = sphereCenter.getVectorX();
		double sphereCenterY = sphereCenter.getVectorY();
		double sphereCenterZ = sphereCenter.getVectorZ();
		
		double a = 1;
		double b = (2*(rayOriginX - sphereCenterX)*rayDirectionX) +
					(2*(rayOriginY - sphereCenterY)*rayDirectionY) +
					(2*(rayOriginZ - sphereCenterZ)*rayDirectionZ);
		double c = pow(rayOriginX - sphereCenterX, 2) +
					pow(rayOriginY - sphereCenterY, 2) +
					pow(rayOriginZ - sphereCenterZ, 2) -
					(radius*radius);
		double discriminant = b*b - 4*c;
		
		if(discriminant > 0) {//intersection!
			double root1 = ((-1*b - sqrt(discriminant))/2);
			double root2 = ((sqrt(discriminant)-b)/2);
			if(root1 > 0) {// use root1
				Vector hit = ray.getRayOrigin().vectorAddition(ray.getRayDirection().vectorScalar(root1));
				double diff = ray.getRayOrigin().vectorAddition(hit.invert()).magnitude();
				if(diff < .001){
						return -1;
				}
				else return root1;
			}
			else {
				Vector hit = ray.getRayOrigin().vectorAddition(ray.getRayDirection().vectorScalar(root2));
				double diff = ray.getRayOrigin().vectorAddition(hit.invert()).magnitude();
				if(diff < .001){
						return -1;
				}
				else return root2;
			}
		}
		else {//miss
			return -1;
		}
	}
};


#endif
