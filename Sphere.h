#ifndef _SPHERE_H
#define _SPHERE_H

#include "Cimg.h"
#include "Vector.h"
#include "Color.h"
#include "Object.h"
#include <math.h>
#include <string>
#include <iostream>

#define M_PI 3.14159265358979323846

class Sphere : public Object {
	Vector center;
	double radius;
	Color color;
	cimg_library::CImg<double> texMap;
	
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
	
	Sphere(Vector a, double b, cimg_library::CImg<double> textureImage ) {
		center = a;
		radius = b;
		color = Color(0.0, 0.0, 0.0, 4.0);
		texMap = textureImage;
	}
	
	Vector getSphereCenter(){
			return center;
	}
    double getSphereRadius(){
			return radius;
	}
	virtual Color getColor(){
			return color;
	}
	
	virtual Color getTexMapColor( Vector intersectionPosition ){
			Vector texCoordinates = getTexCoordinates(intersectionPosition);
			Color texColor = getTexColor( texCoordinates );
			return texColor;
	}
	
	//Based on 3D vector return 2D vector of texture coordinates
	Vector getTexCoordinates(Vector intersectionPosition ){
		//Calculate Vector d - normalized vector from intersection to center
		Vector d = center.vectorAddition( intersectionPosition.invert() );
		d = d.normalize();
		
		//calculate u
		double u = 0.5 + atan2(d.getVectorZ(), d.getVectorX()) / (2 * M_PI);
		
		//calculate v
		double v = 0.5 - asin(d.getVectorY()) / (M_PI);
		
		//std::cout << u << " , " << v << std::endl;
		
		//return ( u , v) vector
		return Vector( u , v , 0.0);
	}
	
	//Based on 2D Vector return color at image coordinates
	Color getTexColor(Vector texCoordinates ){
		//Get unsigned int coordinates based on 2D vector using image's width and height
		unsigned int u = texCoordinates.getVectorX() * texMap.width();
		unsigned int v = texCoordinates.getVectorY() * texMap.height();
		
		double r = texMap(u,v,0)/255;
		double g = texMap(u,v,1)/255;
		double b = texMap(u,v,2)/255;
		
		//std::cout << "r: " << r << " g: " << g << " b: " << b << std::endl;
		
		//Create color from that data
		Color texColor = Color( r, g, b, 4.0);
	
	
		//Return the color
		return texColor;
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
				return root1;
			}
			else {
				return root2;
			}
		}
		else {//miss
			return -1;
		}
	}
};


#endif
