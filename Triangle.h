#ifndef _Triangle_H
#define _Triangle_H

#include "Vector.h"
#include "Color.h"
#include "math.h"
#include "Object.h"
#include "CImg.h"


class Triangle : public Object {
	Vector A, B, C;
	Vector texCoorA, texCoorB, texCoorC;
	double distance;
	Color color;
	cimg_library::CImg<double> texMap;
	
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
	
	Triangle(Vector a, Vector b, Vector c, cimg_library::CImg<double> textureImage, Vector A_texCoor, Vector B_texCoor, Vector C_texCoor ){
	    A = a;
		B = b;
		C = c;
		color = Color(0.0,0.0,0.0,4);
        texMap = textureImage;
		texCoorA = A_texCoor;
		texCoorB = B_texCoor;
		texCoorC = C_texCoor;
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
	
	virtual Color getTexMapColor( Vector intersectionPosition ){
            Vector texCoordinates = getTexCoordinates(intersectionPosition);
            Color texColor = getTexColor( texCoordinates );
            return texColor;
    }
	
	virtual Vector getNormal(Vector point) {
		Vector normal = getTriangleNormal();
		return normal;
	}
	
	//Based on 3D position return 2D vector of texture coordinates
    Vector getTexCoordinates(Vector intersectionPosition ){
        //Interpolation based on barycentric coordinates
		
		//Get sub triangle areas
		Vector normal = getTriangleNormal();
		
		//Area ABC
		Vector BA ( B.getVectorX() - A.getVectorX(), B.getVectorY() - A.getVectorY(), B.getVectorZ() - A.getVectorZ());
		Vector CA ( C.getVectorX() - A.getVectorX(), C.getVectorY() - A.getVectorY(), C.getVectorZ() - A.getVectorZ());
		double areaABC = (BA.crossProduct(CA)).dotProduct(normal);
		
		//Area PBC
		Vector BP ( B.getVectorX() - intersectionPosition.getVectorX(), B.getVectorY() - intersectionPosition.getVectorY(), B.getVectorZ() - intersectionPosition.getVectorZ());
		Vector CP ( C.getVectorX() - intersectionPosition.getVectorX(), C.getVectorY() - intersectionPosition.getVectorY(), C.getVectorZ() - intersectionPosition.getVectorZ());
		double areaPBC = (BP.crossProduct(CP)).dotProduct(normal);
			
		//Area PCA
		//Use Vector CP from Area PBC
		Vector AP ( A.getVectorX() - intersectionPosition.getVectorX(), A.getVectorY() - intersectionPosition.getVectorY(), A.getVectorZ() - intersectionPosition.getVectorZ());
		double areaPCA = (CP.crossProduct(AP)).dotProduct(normal);
		
		double baryx = areaPBC / areaABC; // Alpha - corresponds to point A 
		double baryy = areaPCA / areaABC; // Beta  - corresponds to point B
		double baryz = 1.0f - baryx - baryy; // gamma - corresponds to point C
		
		//tex coordinate vector's for each point have X and Y corresponding to u , v text map coordinates.
		//Interpolate those 2D tex coordinate values for each of the three points based on the barycentric coordinates of intersection point
		double u = texCoorA.getVectorX()* baryx + texCoorB.getVectorX()*baryy + texCoorC.getVectorX() * baryz;
		double v = texCoorA.getVectorY()* baryx + texCoorB.getVectorY()*baryy + texCoorC.getVectorY() * baryz;
		
		return Vector( u, v, 0.0);
		
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
