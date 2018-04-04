#include "CImg.h"
using namespace cimg_library;
int main() {
  CImg<unsigned char> image("spheres.bmp"), visu(500,400,1,3,0);
  const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };
  image.blur(0);
  CImgDisplay main_disp(image,"Click a point");
  while (!main_disp.is_closed()) {
    main_disp.wait();
    
    }
  return 0;
}