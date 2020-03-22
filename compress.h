#ifndef COMPRESS_H
#define COMPRESS_H
#include "ima.h"
#include <math.h>

#define RED 0
#define GREEN 1
#define BLUE 2
#define S 0
#define V 1

struct Image_compressed
{
    unsigned long sizeX;
    unsigned long sizeY;
    unsigned long * sizeChannel;
    GLbyte **data;
};
typedef struct Image_compressed Image_compressed;

struct Image_HSV
{
    unsigned long sizeX;
    unsigned long sizeY;
    GLshort * Hdata;
    GLubyte **SVdata;
};
typedef struct Image_HSV Image_HSV;


unsigned long compress_RGB(Image img, Image_compressed * dst,unsigned long sizeX, unsigned long sizeY, int color);
GLbyte * reduce_raw_compressed(GLbyte* raw_compressed, unsigned long *size);


Image_compressed compress(Image img);

void save_compressed_image(char * filename,Image_compressed * img);


void print_image(Image img);
void rgb_to_hsv(GLubyte r, GLubyte g, GLubyte b, short * h, GLubyte * s, GLubyte * v);
Image_HSV conv_RGB_img_to_HSV_img(Image src);
void conv_RGB_HSV(Image src, Image_HSV dst);
#endif