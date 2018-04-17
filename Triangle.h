#ifndef _Triangle_H
#define _Triangle_H

#include "Vector.h"
#include "Color.h"
#include "math.h"
#include "Object.h"

class Triangle : public Object {
	Vector A, B, C;
	double distance;
	Color color;
	
	public:
	
	Triangle(){
		A = Vector(1,0,0);
		B = Vector(0,1,0);
		C = Vector(0,0,1);
		color = Color(0.5, 0.5, 0.5, 0);
	}
	
	Triangle(Vector a, Vector b, Vector c, Color colorValue){
		A = a;
		B = b;
		C = c;
		color = colorValue;
	}

	
	Vector getTriangleNormal(){
		Vector CA ( C.getVectorX() - A.getVectorX(), C.getVectorY() - A.getVectorY(), C.getVectorZ() - A.getVectorZ());
		Vector BA ( B.getVectorX() - A.getVectorX(), B.getVectorY() - A.getVectorY(), B.getVectorZ() - A.getVectorZ());
		Vector normal = CA.crossProduct(BA).normalize();
		return normal;
	}
    double getTriangleDistance(){
    	Vector normal = getTriangleNormal();
    	double distance = normal.dotProduct(A);
		return distance;
	}
	virtual Color getColor(){
		return color;
	}
	
	virtual Vector getNormal(Vector point) {
		Vector normal = getTriangleNormal();
		return normal;
	}
	
	virtual double intersection(Ray ray) {
		Vector rayDirection = ray.getRayDirection();
		Vector ray_origin = ray.getRayOrigin();
		
		Vector normal = getTriangleNormal();
		double distance = getTriangleDistance();
		
		double m = rayDirection.dotProduct(normal);
		if(m == 0){//parallel to Triangle
			return -1;
		}
		else {
			double n = normal.dotProduct(ray.getRayOrigin().vectorAddition(normal.vectorScalar(distance).invert()));
			double distanceToPlane = -1*n/m;
			
			double intersectionX = rayDirection.vectorScalar(distanceToPlane).getVectorX() + ray_origin.getVectorX();
			double intersectionY = rayDirection.vectorScalar(distanceToPlane).getVectorY() + ray_origin.getVectorY();
			double intersectionZ = rayDirection.vectorScalar(distanceToPlane).getVectorZ() + ray_origin.getVectorZ();
			Vector D(intersectionX, intersectionY, intersectionZ);
			
			//CA
			Vector CA ( C.getVectorX() - A.getVectorX(), C.getVectorY() - A.getVectorY(), C.getVectorZ() - A.getVectorZ());
			Vector DA ( D.getVectorX() - A.getVectorX(), D.getVectorY() - A.getVectorY(), D.getVectorZ() - A.getVectorZ());
			double check1 = (CA.crossProduct(DA)).dotProduct(normal);
			//BC
			Vector BC ( B.getVectorX() - C.getVectorX(), B.getVectorY() - C.getVectorY(), B.getVectorZ() - C.getVectorZ());
			Vector DC ( D.getVectorX() - C.getVectorX(), D.getVectorY() - C.getVectorY(), D.getVectorZ() - C.getVectorZ());
			double check2 = (BC.crossProduct(DC)).dotProduct(normal);
			
			//AB
			Vector AB ( A.getVectorX() - B.getVectorX(), A.getVectorY() - B.getVectorY(), A.getVectorZ() - B.getVectorZ());
			Vector DB ( D.getVectorX() - B.getVectorX(), D.getVectorY() - B.getVectorY(), D.getVectorZ() - B.getVectorZ());
			double check3 = (AB.crossProduct(DB)).dotProduct(normal);
			
			if(check1 >= 0 && check2 >= 0 && check3 >=0){
				return distanceToPlane;
			}
			else{
				return -1;
			}
		}
			
	}
	
};

#endif
