#ifndef _COLOR_H
#define _COLOR_H

#include "math.h"


class Color {
	double red, green, blue, special;
	
	public:
	
	Color(){
		red=0.5;
		green=0.5;
		blue=0.5;
	}
	
	Color(double r, double g, double b, double s){
		red = r;
		green = g;
		blue = b;
		special = s;
	}
	
	double getColorRed(){
			return red;
	}
	double getColorGreen(){
			return green;
	}
	double getColorBlue(){
			return blue;
	}
	double getColorSpecial(){
			return special;
	}
	
	double setRed(double value) {
		red = value;
	};
	double setGreen(double value) {
		green = value;
	};
	double setBlue(double value) {
		blue = value;
	};
	double setSpecial(double value) {
		special = value;
	};
	
	double brightness() {
		return ((red + green + blue)/3);
	}
	
	Color colorScalar(double scalar) {
		return Color (red*scalar, green*scalar, blue*scalar, special);
	}
	
	Color colorAddition(Color color) {
		return Color (red + color.getColorRed(), 
						green + color.getColorGreen(),
						blue + color.getColorBlue(),
						special);
	}
	
	Color colorMultiply(Color color) {
		return Color (red * color.getColorRed(), 
						green * color.getColorGreen(),
						blue * color.getColorBlue(),
						special);
	}
	
	Color colorMean(Color color) {
		return Color ((red + color.getColorRed())/2, 
						(green + color.getColorGreen())/2,
						(blue + color.getColorBlue())/2,
						special);
	}
	
	Color cutoff() {
		double totalLight = red + green + blue;
		double extraLight = totalLight - 3;
		if(extraLight > 0) {
			red = red + extraLight*(red/totalLight);
			green = green + extraLight*(green/totalLight);
			blue = blue + extraLight*(blue/totalLight);
		}
		if(red > 1) {
			red = 1;
		}
		if(green > 1) {
			green = 1;
		}
		if(blue > 1) {
			blue = 1;
		}
		if(red < 0){
			red = 0;
		}
		if(green < 0){
			green = 0;
		}
		if(blue < 0){
			blue = 0;
		}
		
		return Color(red, green, blue, special);
	}
						
};


#endif