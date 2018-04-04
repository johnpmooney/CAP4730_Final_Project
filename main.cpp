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
#include "CImg.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

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

//used to make shadows
Color getColorAt(Vector intersectionPosition, Vector intersectionRayDirection, std::vector<Object*> sceneObjects, int closestObject, std::vector<Source*> lightSources, double adjust, double ambientlight) {
	
	Color closestObjectColor = sceneObjects.at(closestObject)->getColor();
	Vector closestObjectNormal = sceneObjects.at(closestObject)->getNormal(intersectionPosition);
	Color finalColor = closestObjectColor.colorScalar(ambientlight);
	
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


int main(int argc, char *argv[]) {

	int dpi = 72;
	int width = 640;
	int height = 480;
	int n = width * height;
	RGB *pixels = new RGB[n];
	
	double aspectRatio = (double)width/(double)height;
	double ambientlight = 0.2;
	double adjust = 0.00001;
	
	
	//Vector set up
	Vector Origin (0,0,0);
	Vector X (1,0,0);
	Vector Y (0,1,0);
	Vector Z (0,0,1);
	
	
	//Camera set up
	Vector cameraPosition (0,1.5,-4);
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
	Color red (1.0,0,0,0.3);
	Color green (0,1.0,0,0);
	Color blue (0,0,1.0,0);
	Color gray (0.5,0.5,0.5,0);
	Color black (0,0,0,0);
	
	
	//Lighting
	Vector lightPosition (0,2,-1);
	Light sceneLight ( lightPosition, whitLight);
	std::vector<Source*> lightSources;
	lightSources.push_back(dynamic_cast<Source*>(&sceneLight));
	
	
	//Objects in the Scene
	Vector s1 (1.25,0,0);
	Vector s2 (-1,0,0);
	Sphere sceneSphere (s1,1, red);
	Sphere sceneSphere1 (s2,1,blue);
	Plane scenePlane (Y, -1, gray);
	
	std::vector<Object*> sceneObjects;
	sceneObjects.push_back(dynamic_cast<Object*>(&sceneSphere));
	sceneObjects.push_back(dynamic_cast<Object*>(&sceneSphere1));
	sceneObjects.push_back(dynamic_cast<Object*>(&scenePlane));
	
	double xAmount, yAmount;

	

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			here = y * width + x;
			
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
				pixels[here].R = 0;
				pixels[here].G = 0;
				pixels[here].B = 0;
			}
			else { // hit, rendering objects!
				if(intersections.at(closetObject) > adjust){
					Vector intersectionPosition = cameraRayOrigin.vectorAddition(cameraRayDirection.vectorScalar(intersections.at(closetObject)));
					Vector intersectionRayDirection = cameraRayDirection;
					
					Color thisColor = getColorAt(intersectionPosition, intersectionRayDirection, sceneObjects, closetObject, lightSources, adjust, ambientlight);
				
					pixels[here].R = thisColor.getColorRed();
					pixels[here].G = thisColor.getColorGreen();
					pixels[here].B = thisColor.getColorBlue();
				}
				
			}

		}
	}
	
	bitmap("spheres.bmp", width, height, pixels);
	
	CImg<unsigned char> image("spheres.bmp"), visu(500,400,1,3,0);
	const unsigned char redColor[] = { 255,0,0 }, greenColor[] = { 0,255,0 }, blueColor[] = { 0,0,255 };
	image.blur(0);
	CImgDisplay main_disp(image,"Click a point");
	while (!main_disp.is_closed()) {
		main_disp.wait();
		
	}
	 return 0; 


}
