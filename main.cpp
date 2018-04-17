#include <stdio.h>
#include <stdlib.h>
#include "Vector.h" 
#include "Ray.h"
#include "Camera.h"
#include "Color.h"
#include "Light.h"
#include "Sphere.h"
#include "Object.h"
#include "Source.h"
#include "Plane.h"
#include "Triangle.h"
#include "CImg.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <bits/stdc++.h>

using namespace cimg_library;

int here;

struct RGB {
	double R;
	double G;
	double B;
};

//this bitmap was used from https://stackoverflow.com/questions/30423590/generate-bmp-file-from-array-of-rgb-values
//I have made a few moditifications to suit my needs
void bitmap(const char *filename, int width,int height,RGB *data)
{
typedef struct                       /**** BMP file header structure ****/
    {
    unsigned int   bfSize;           /* Size of file */
    unsigned short bfReserved1;      /* Reserved */
    unsigned short bfReserved2;      /* ... */
    unsigned int   bfOffBits;        /* Offset to bitmap data */
    } BITMAPFILEHEADER;

typedef struct                       /**** BMP file info structure ****/
    {
    unsigned int   biSize;           /* Size of info header */
    int            biWidth;          /* Width of image */
    int            biHeight;         /* Height of image */
    unsigned short biPlanes;         /* Number of color planes */
    unsigned short biBitCount;       /* Number of bits per pixel */
    unsigned int   biCompression;    /* Type of compression to use */
    unsigned int   biSizeImage;      /* Size of image data */
    int            biXPelsPerMeter;  /* X pixels per meter */
    int            biYPelsPerMeter;  /* Y pixels per meter */
    unsigned int   biClrUsed;        /* Number of colors used */
    unsigned int   biClrImportant;   /* Number of important colors */
    } BITMAPINFOHEADER;

BITMAPFILEHEADER bfh;
BITMAPINFOHEADER bih;

/* Magic number for file. It does not fit in the header structure due to alignment requirements, so put it outside */
unsigned short bfType=0x4d42;           
bfh.bfReserved1 = 0;
bfh.bfReserved2 = 0;
bfh.bfSize = 2+sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)+width*height*3;
bfh.bfOffBits = 0x36;

bih.biSize = sizeof(BITMAPINFOHEADER);
bih.biWidth = width;
bih.biHeight = height;
bih.biPlanes = 1;
bih.biBitCount = 24;
bih.biCompression = 0;
bih.biSizeImage = 0;
bih.biXPelsPerMeter = 5000;
bih.biYPelsPerMeter = 5000;
bih.biClrUsed = 0;
bih.biClrImportant = 0;

FILE *file = fopen(filename, "wb");
if (!file)
    {
    printf("Could not write file\n");
    return;
    }




/*Write headers*/
fwrite(&bfType,1,sizeof(bfType),file);
fwrite(&bfh, 1, sizeof(bfh), file);
fwrite(&bih, 1, sizeof(bih), file);

/*Write bitmap*/
int k = width*height;
for (int i = 0;i < k; i++) {
		RGB rgb = data[i];

		double red = (data[i].R) * 255;
		double green = (data[i].G) * 255;
		double blue = (data[i].B) * 255;

		unsigned char color[3] = { (int)floor(blue), (int)floor(green), (int)floor(red) };

		fwrite(color, 1, 3, file);

	}
fclose(file);
}

int closestObjectIndex(std::vector<double> objectIntersections) {
	int min;
	
	if(objectIntersections.size() == 0) {
		return -1;
	}
	else if(objectIntersections.size() == 1) {// one hit
		if(objectIntersections.at(0) > 0) {//positive
			return 0;
		}
		else {//ray missed
			return -1;
		}
	}
	else {//multiple hits
		double max = 0;
		for(int i =0; i < objectIntersections.size(); i++) {//find max
			if (max < objectIntersections.at(i)) {
				max = objectIntersections.at(i);
			}
		}
		
		if(max > 0) { // finding min
			for(int i = 0; i < objectIntersections.size(); i++) {
				if(objectIntersections.at(i) > 0 && objectIntersections.at(i) <= max) {
					max = objectIntersections.at(i);
					min = i;
				}
			}
			return min;
		}
		else {// no hits all negative
			return -1;
		}
	
	}
}

float clip(float n, float lower, float upper) {
  return std::max(lower, std::min(n, upper));
}

double fresnel(Vector Ri, Vector normal, double ior) {
	double c1 = Ri.dotProduct(normal);
	double cosi = clip(-1,1,c1);
	double etai = 1;
	double etat = ior;
	if(cosi > 0){
		etai = etat;
		etat = 1;
	}
	double sint = etai/etat * sqrt(std::max((double)0,1-cosi*cosi));
	if(sint >= 1) {
		return 1;
	}
	else {
		double cost = sqrt(std::max((double)0,1-sint*sint));
		cosi = fabs(cosi);
		double Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
		double Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
		return (Rs * Rs + Rp * Rp) / 2;
	}
}

Color getColorAt(Ray camera, Vector intersectionPosition, Vector intersectionRayDirection, std::vector<Object*> sceneObjects, int closestObject, std::vector<Source*> lightSources, double adjust, double ambientlight) {
	
	Color closestObjectColor = sceneObjects.at(closestObject)->getColor();
	Vector closestObjectNormal = sceneObjects.at(closestObject)->getNormal(intersectionPosition);
	
	//tile black and white
	if(closestObjectColor.getColorSpecial() == 5){
		
		int square = (int)floor(intersectionPosition.getVectorX()) + (int)floor(intersectionPosition.getVectorZ());
		
		if((square % 2) == 0){
			//black
			closestObjectColor.setRed(0);
			closestObjectColor.setGreen(0);
			closestObjectColor.setBlue(0);
		}
		else{
			//white
			closestObjectColor.setRed(1);
			closestObjectColor.setGreen(1);
			closestObjectColor.setBlue(1);
		}
		
	}
	
	Color finalColor = closestObjectColor.colorScalar(ambientlight);
	
	//reflections
	if(closestObjectColor.getColorSpecial() > 0 and closestObjectColor.getColorSpecial() <=1){
		//used for reflections on a shiny objects
		Vector RiDirection = camera.getRayDirection();
		Vector normal = closestObjectNormal;
		double dotandS = 2 * normal.dotProduct(RiDirection);
		Vector scale = normal.vectorScalar(dotandS);
		Vector reflectionDirection = RiDirection.vectorAddition(scale.invert());
		
		Ray reflectionRay (intersectionPosition, reflectionDirection);
		
		//array for first hits for the ray
		std::vector<double> reflectionIntersections;
		
		for(int r_index = 0; r_index < sceneObjects.size(); r_index++) {
			reflectionIntersections.push_back(sceneObjects.at(r_index)->intersection(reflectionRay));
		}
		
		int closestObjectReflecting_index = closestObjectIndex(reflectionIntersections);
		
		if(closestObjectReflecting_index != -1 ) {
			
			if(reflectionIntersections.at(closestObjectReflecting_index) > adjust) {
				//find pos and dir of reflection intersection
				Vector reflectionIntersectionPosition = intersectionPosition.vectorAddition(reflectionDirection.vectorScalar(reflectionIntersections.at(closestObjectReflecting_index)));
				Vector reflectionIntersectionRayDirection = reflectionDirection;
				
				//recursion
				Color reflectionIntersectionColor = getColorAt(camera, reflectionIntersectionPosition, reflectionIntersectionRayDirection, sceneObjects, closestObjectReflecting_index, lightSources, adjust, ambientlight);   
		
				finalColor = finalColor.colorAddition(reflectionIntersectionColor.colorScalar(closestObjectColor.getColorSpecial()));
			}
		}	
	}
	
	//refractions
	if(closestObjectColor.getColorSpecial() > 1 and closestObjectColor.getColorSpecial() <=2) {
		Vector RiDirection = camera.getRayDirection();
		Vector normal = closestObjectNormal;
		double etai = 1;
		double etat = closestObjectColor.getColorSpecial();
		double n = 1/closestObjectColor.getColorSpecial();
		double c1 = normal.dotProduct(RiDirection);
		double cosi = clip(c1,-1,1);
		if (cosi > 0) {
			etai = closestObjectColor.getColorSpecial();
			etai = 1;
			normal = normal.invert();
		} else { 
			cosi = -cosi; 
		}
		double eta = etai/etat;
		double c2 = 1 - eta * eta *(1 - cosi * cosi);
		if(c2 >= 0){
			c2 = (double)sqrt(1 - n * n *(1 - cosi * cosi));
			Vector scale1 = RiDirection.vectorScalar(n);
			Vector scale2 = normal.vectorScalar(eta * cosi - c2);
			Vector refractionDirection = scale1.vectorAddition(scale2);
				
			Ray refractionRay (intersectionPosition, refractionDirection);
			
			std::vector<double> refractionIntersections;
			
			for(int r_index = 0; r_index < sceneObjects.size(); r_index++) {
				refractionIntersections.push_back(sceneObjects.at(r_index)->intersection(refractionRay));
			}
				
			int closestObjectRefraction_index = closestObjectIndex(refractionIntersections);
				
			if(closestObjectRefraction_index != -1 ) {
					
				if(refractionIntersections.at(closestObjectRefraction_index) > adjust) {
					//find pos and dir of refraction ray
					Vector refractionIntersectionPosition = intersectionPosition.vectorAddition(refractionDirection.vectorScalar(refractionIntersections.at(closestObjectRefraction_index)));
					Vector refractionIntersectionRayDirection = refractionDirection;
						
					//recursion
					Color refractionIntersectionColor = getColorAt(camera, refractionIntersectionPosition, refractionIntersectionRayDirection, sceneObjects, closestObjectRefraction_index, lightSources, adjust, ambientlight);   
					
					finalColor = finalColor.colorAddition(refractionIntersectionColor.colorScalar(closestObjectColor.getColorSpecial()));
				}
			}
		}	
	}
	
	//fresnel
	if(closestObjectColor.getColorSpecial() > 2 and closestObjectColor.getColorSpecial() <=3) {
		
		Color reflectionIntersectionColor;
		Color refractionIntersectionColor;
		
		double ior = closestObjectColor.getColorSpecial() - 1;
		double fres = fresnel(camera.getRayDirection(),closestObjectNormal, ior);
		
		if(fres < 1){
			Vector RiDirection = camera.getRayDirection();
			Vector normal = closestObjectNormal;
			double etai = 1;
			double etat = closestObjectColor.getColorSpecial();
			double n = 1/closestObjectColor.getColorSpecial();
			double c1 = normal.dotProduct(RiDirection);
			double cosi = clip(c1,-1,1);
			if (cosi > 0) {
				etai = closestObjectColor.getColorSpecial();
				etai = 1;
				normal = normal.invert();
			} else { 
				cosi = -cosi; 
			}
			double eta = etai/etat;
			double c2 = 1 - eta * eta *(1 - cosi * cosi);
			if(c2 >= 0){
				c2 = (double)sqrt(1 - n * n *(1 - cosi * cosi));
				Vector scale1 = RiDirection.vectorScalar(n);
				Vector scale2 = normal.vectorScalar(eta * cosi - c2);
				Vector refractionDirection = scale1.vectorAddition(scale2);
					
				Ray refractionRay (intersectionPosition, refractionDirection);
				
				std::vector<double> refractionIntersections;
				
				for(int r_index = 0; r_index < sceneObjects.size(); r_index++) {
					refractionIntersections.push_back(sceneObjects.at(r_index)->intersection(refractionRay));
				}
					
				int closestObjectRefraction_index = closestObjectIndex(refractionIntersections);
					
				if(closestObjectRefraction_index != -1 ) {
						
					if(refractionIntersections.at(closestObjectRefraction_index) > adjust) {
						//find pos and dir of refraction ray
						Vector refractionIntersectionPosition = intersectionPosition.vectorAddition(refractionDirection.vectorScalar(refractionIntersections.at(closestObjectRefraction_index)));
						Vector refractionIntersectionRayDirection = refractionDirection;
							
						//recursion
						refractionIntersectionColor = getColorAt(camera, refractionIntersectionPosition, refractionIntersectionRayDirection, sceneObjects, closestObjectRefraction_index, lightSources, adjust, ambientlight);   
						
						//finalColor = finalColor.colorAddition(refractionIntersectionColor.colorScalar(closestObjectColor.getColorSpecial()));
					}
				}
			}
			
		}
			//used for reflections on a shiny objects
		Vector RiDirection = camera.getRayDirection();
		Vector normal = closestObjectNormal;
		double dotandS = 2 * normal.dotProduct(RiDirection);
		Vector scale = normal.vectorScalar(dotandS);
		Vector reflectionDirection = RiDirection.vectorAddition(scale.invert());
		
		Ray reflectionRay (intersectionPosition, reflectionDirection);
		
		//array for first hits for the ray
		std::vector<double> reflectionIntersections;
		
		for(int r_index = 0; r_index < sceneObjects.size(); r_index++) {
			reflectionIntersections.push_back(sceneObjects.at(r_index)->intersection(reflectionRay));
		}
		
		int closestObjectReflecting_index = closestObjectIndex(reflectionIntersections);
		
		if(closestObjectReflecting_index != -1 ) {
			
			if(reflectionIntersections.at(closestObjectReflecting_index) > adjust) {
				//find pos and dir of reflection intersection
				Vector reflectionIntersectionPosition = intersectionPosition.vectorAddition(reflectionDirection.vectorScalar(reflectionIntersections.at(closestObjectReflecting_index)));
				Vector reflectionIntersectionRayDirection = reflectionDirection;
				
				//recursion
				reflectionIntersectionColor = getColorAt(camera, reflectionIntersectionPosition, reflectionIntersectionRayDirection, sceneObjects, closestObjectReflecting_index, lightSources, adjust, ambientlight);   
		
				//finalColor = finalColor.colorAddition(reflectionIntersectionColor.colorScalar(closestObjectColor.getColorSpecial()));
			}
		}
		
		Color reflect = finalColor.colorAddition(reflectionIntersectionColor.colorScalar(closestObjectColor.getColorSpecial()-2));
		Color refract = finalColor.colorAddition(refractionIntersectionColor.colorScalar(closestObjectColor.getColorSpecial()-1));
		reflect = reflect.colorScalar(fres);
		refract = refract.colorScalar((double)1 - fres);
		Color mix = reflect.colorAddition(refract);
		finalColor = finalColor.colorAddition(mix);
		
	}

	
	for(int i = 0; i < lightSources.size(); i++) {
		Vector lightDirection = lightSources.at(i)->getLightPosition().vectorAddition(intersectionPosition.invert()).normalize();
		
		float cos = closestObjectNormal.dotProduct(lightDirection);
		
		if(cos > 0) {//check for shadows,spectral
			bool shadow = false;
			
			Vector distanceToLight = lightSources.at(i)->getLightPosition().vectorAddition(intersectionPosition.invert()).normalize();
			float distanceToLightMagnitude = distanceToLight.magnitude();
			
			Ray shadowRay (intersectionPosition, lightSources.at(i)->getLightPosition().vectorAddition(intersectionPosition.invert()).normalize());
			
			std::vector<double> intersectionValues;
			
			for(int j = 0; j < sceneObjects.size() && shadow == false; j++) {
				intersectionValues.push_back(sceneObjects.at(j)->intersection(shadowRay));
			}
			for(int k = 0; k < intersectionValues.size(); k++) {
				if(intersectionValues.at(k) > adjust){
					if(intersectionValues.at(k) <= distanceToLightMagnitude) {
						shadow = true;
					}
					break;	
				}
			}
			
			if(shadow == false) {
				finalColor = finalColor.colorAddition(closestObjectColor.colorMultiply(lightSources.at(i)->getLightColor()).colorScalar(cos));
				
				double spectralColor = closestObjectColor.getColorSpecial();
				
				
				if(spectralColor > 0 && spectralColor <= 1) {//spectral
					double dot1 = closestObjectNormal.dotProduct(intersectionRayDirection.invert());
					Vector scalar1 = closestObjectNormal.vectorScalar(dot1);
					Vector add1 = scalar1.vectorAddition(intersectionRayDirection);
					Vector scalar2 = add1.vectorScalar(2);
					Vector add2 = intersectionRayDirection.invert().vectorAddition(scalar2);
					Vector reflectionDirection = add2.normalize();
					
					double spectral = reflectionDirection.dotProduct(lightDirection);
					if(spectral > 0) {
						spectral = pow(spectral, 10);
						finalColor = finalColor.colorAddition(lightSources.at(i)->getLightColor().colorScalar(spectral*closestObjectColor.getColorSpecial()));
					}
				}
			}
		}
			
	}
	
	return finalColor.cutoff();
}

std::vector<Object*> sceneObjects; // object Vector for the whole scene

void tetrahedron(Vector ptA, Vector ptB, Vector ptC,double height,Color color){
	
	double x = (ptA.getVectorX()+ptB.getVectorX()+ptC.getVectorX())/3;
	double y = 1 * height;
	double z = (ptA.getVectorZ()+ptB.getVectorZ()+ptC.getVectorZ())/3;
	
	Vector ptD (x,y,z);
	
	Color red (1.0,0,0,0);
	Color green (0,1.0,0,0);
	Color blue (0,0,1.0,0);
	
	sceneObjects.push_back(new Triangle(ptA,ptB,ptC,color));//base
	sceneObjects.push_back(new Triangle (ptA,ptB,ptD,red));
	sceneObjects.push_back(new Triangle (ptB,ptC,ptD,green));
	sceneObjects.push_back(new Triangle (ptC,ptA,ptD,blue));
	
}

void box(Vector pt1,Vector pt2,double scale, Color color){//ptA and ptB are oppisite corners
	
	double pt1X = pt1.getVectorX();
	double pt1Y = pt1.getVectorY();
	double pt1Z = pt1.getVectorZ();
	
	double pt2X = pt2.getVectorX();
	double pt2Y = pt2.getVectorY();
	double pt2Z = pt2.getVectorZ();
	
	Vector A (pt2X,pt1Y,pt1Z);
	Vector B (pt2X,pt1y,pt2Z);
	Vector C (pt1X,pt1Y,pt2Z);
	
	Vector D (pt2X,pt2Y,pt1Z);
	Vector E (pt1X,pt2Y,pt1Z);
	Vector F (pt1X,pt2Y,pt2Z);
	
	sceneObjects.push_back(new Triangle (D,A,pt1,color));
	
}
	

int main(int argc, char *argv[]) {

	int width = 640;
	int height = 480;
	int n = width * height;
	RGB *pixels = new RGB[n];
	
	int AntiDepth = 4;
	double AntiThreshold = 0.1;
	
	double aspectRatio = (double)width/(double)height;
	double ambientlight = 0.2;
	double adjust = 0.00000001;
	
	
	//Vector set up
	Vector Origin (0,0,0);
	Vector X (1,0,0);
	Vector Y (0,1,0);
	Vector Z (0,0,1);
	
	
	//Camera set up
	Vector cameraPosition (0,0.2,-4); //main view (0,0.2,-4)
	Vector lookAt (0,0,0);
	Vector difference_btw (cameraPosition.getVectorX() - lookAt.getVectorX(),
							cameraPosition.getVectorY() - lookAt.getVectorY(),
							cameraPosition.getVectorZ() - lookAt.getVectorZ());
							
	Vector cameraDirection = difference_btw.invert().normalize();
	Vector cameraRight = Y.crossProduct(cameraDirection).normalize();
	Vector cameraDown = cameraRight.crossProduct(cameraDirection);
	Camera sceneCamera (cameraPosition, cameraDirection, cameraRight, cameraDown);
	
	//Colors
	Color whitLight (1.0,1.0,1.0, 0);
	Color red (1.0,0,0,.2);
	Color green (0,1.0,0,0);
	Color blue (0,0,1.0,1);
	Color gray (0.5,0.5,0.5,0);
	Color black (0,0,0,0);
	Color tile (1.0,1.0,1.0,5);
	Color clear (0.0,0.0,0.0, 1.3);
	Color fres (0.5,0.5,0.5,2.5);
	
	
	//Lighting
	Vector lightPosition (-5,15,-7);
	Light sceneLight ( lightPosition, whitLight);
	std::vector<Source*> lightSources;
	lightSources.push_back(dynamic_cast<Source*>(&sceneLight));
	
	
	//Objects in the Scene
	Vector s1 (1.25,0,0);
	Vector s2 (-1,0,0);
	Vector A1 (1,3,-2);
	Vector B1 (3,-1,0);
	Vector C1 (-3,-1,0);
	Sphere sceneSphere (s1,1, clear);
	Sphere sceneSphere1 (s2,1,blue);
	Triangle sceneTriangle (A1, B1, C1, red);
	Plane scenePlane (Y, -1, tile);
	
	
	//sceneObjects.push_back(dynamic_cast<Object*>(&sceneSphere));
	//sceneObjects.push_back(dynamic_cast<Object*>(&sceneSphere1));
	//sceneObjects.push_back(dynamic_cast<Object*>(&sceneTriangle));
	sceneObjects.push_back(dynamic_cast<Object*>(&scenePlane));
	
	Vector A2(-1,-1,0);
	Vector B2(1,-1,0);
	Vector C2(0,-1,-1);
	tetrahedron(A2,B2,C2,1,red);
	
	double xAmount, yAmount;
	int AntiIndex;

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			here = y * width + x;
			
			
			//anti-aliasing
			int size = AntiDepth * AntiDepth;
			double tempRed[size];
			double tempGreen[size];
			double tempBlue[size];
			
			
			
			for(int AntiX = 0; AntiX < AntiDepth; AntiX++) {
				for(int AntiY = 0; AntiY < AntiDepth; AntiY++) {
					
					AntiIndex = AntiY*AntiDepth + AntiX;
					
					if(AntiDepth == 1){	//no Anti-aliasing				
						//image plane
						if( width > height){//wide image
							xAmount = ((x+0.5)/width)*aspectRatio - ((width-height)/(double)height/2);
							yAmount = ((height - y) + 0.5)/height;
						}
						else if(height > width){//tall image
							xAmount = (x + 0.5)/width;
							yAmount = (((height - y) + 0.5)/height)/aspectRatio - (((height - width)/(double)width)/2);
						}
						else {//width = height
							xAmount = (x + 0.5)/width;
							yAmount = ((height - y)+0.5)/height;
						}
					}
					else {
							//image plane
						if( width > height){//wide image
							xAmount = ((x + (double)AntiX/((double)AntiDepth-1))/width)*aspectRatio - ((width-height)/(double)height/2);
							yAmount = ((height - y) + (double)AntiX/((double)AntiDepth-1))/height;
						}
						else if(height > width){//tall image
							xAmount = (x + (double)AntiX/((double)AntiDepth-1))/width;
							yAmount = (((height - y) + (double)AntiX/((double)AntiDepth-1))/height)/aspectRatio - (((height - width)/(double)width)/2);
						}
						else {//width = height
							xAmount = (x + (double)AntiX/((double)AntiDepth-1))/width;
							yAmount = ((height - y)+(double)AntiX/((double)AntiDepth-1))/height;
						}
					}

					
					//Creating rays
					Vector cameraRayOrigin = sceneCamera.getCameraPosition();
					Vector cameraRayDirection = cameraDirection.vectorAddition(cameraRight.vectorScalar(xAmount - 0.5).vectorAddition(cameraDown.vectorScalar(yAmount - 0.5))).normalize();
					Ray cameraRay (cameraRayOrigin, cameraRayDirection);
					
					//checking for intersections
					std::vector<double> intersections;
					for(int i = 0; i < sceneObjects.size(); i++) {//loops to find intersections
						intersections.push_back(sceneObjects.at(i)->intersection(cameraRay));
					}
					
					//closest object to camera
					int closetObject = closestObjectIndex(intersections);
					
		
					if (closetObject == -1) {//black background
						tempRed[AntiIndex] = 0;
						tempGreen[AntiIndex] = 0;
						tempBlue[AntiIndex] = 0;
					}
					else { // hit, rendering objects!
						if(intersections.at(closetObject) > adjust){
							Vector intersectionPosition = cameraRayOrigin.vectorAddition(cameraRayDirection.vectorScalar(intersections.at(closetObject)));
							Vector intersectionRayDirection = cameraRayDirection;
							
							Color thisColor = getColorAt( cameraRay,intersectionPosition, intersectionRayDirection, sceneObjects, closetObject, lightSources, adjust, ambientlight);
						
							tempRed[AntiIndex] = thisColor.getColorRed();
							tempGreen[AntiIndex] = thisColor.getColorGreen();
							tempBlue[AntiIndex] = thisColor.getColorBlue();
						}
					
					}

				}
			}
			
			double redSum = 0;
			double greenSum = 0;
			double blueSum = 0;
			
			for(int ir = 0; ir < size; ir++){
				redSum += tempRed[ir];
			}
			for(int ig = 0; ig < size; ig++){
				greenSum += tempGreen[ig];
			}
			for(int ib = 0; ib < size; ib++){
				blueSum += tempBlue[ib];
			}
			
			double averageRed = redSum/size;
			double averageGreen = greenSum/size;
			double averageBlue = blueSum/size;
			
			pixels[here].R = averageRed;
			pixels[here].G = averageGreen;
			pixels[here].B = averageBlue;

		}
	}
	
	bitmap("spheres.bmp", width, height, pixels);
	
	delete pixels;
	
	CImg<unsigned char> image("spheres.bmp"), visu(500,400,1,3,0);
	const unsigned char redColor[] = { 255,0,0 }, greenColor[] = { 0,255,0 }, blueColor[] = { 0,0,255 };
	image.blur(0);
	CImgDisplay main_disp(image,"Click a point");
	while (!main_disp.is_closed()) {
		main_disp.wait();
		
	}
	 return 0; 


}
