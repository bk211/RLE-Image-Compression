#ifndef COMPRESS_H
#define COMPRESS_H
#include "ima.h"
#include <math.h>

#define RED 0
#define GREEN 1
#define BLUE 2
#define S 0
#define V 1
#define SV 0

#define STEP_RGB 3
#define STEP_HSV 1

struct Image_RGB_compressed
{
    unsigned long sizeX;
    unsigned long sizeY;
    unsigned long * sizeChannel;
    GLubyte **data;
};
typedef struct Image_RGB_compressed Image_RGB_compressed;

struct Image_HSV
{
    unsigned long sizeX;
    unsigned long sizeY;
    GLshort * Hdata;
    GLubyte **SVdata;
};
typedef struct Image_HSV Image_HSV;

struct Image_HSV_compressed
{
    unsigned long sizeX;
    unsigned long sizeY;
    unsigned long * sizeChannel;
    GLshort * Hdata;
    GLubyte **SVdata;
};
typedef struct Image_HSV_compressed Image_HSV_compressed;


unsigned long compress_RGB(Image img, Image_RGB_compressed * dst, int color);
GLbyte * reduce_raw_compressed(GLbyte* raw_compressed, unsigned long *size);


Image_RGB_compressed create_compressed_image_from_RGB(Image img);

void save_compressed_image(char * filename,Image_RGB_compressed * img);
unsigned long compress_GLubyte(GLubyte * data, GLbyte * storage, int type, unsigned long size, unsigned long img_type);
unsigned long compress_GLshort(GLshort * data, GLshort * storage, unsigned long size);

void print_image(Image img);
void rgb_to_hsv(GLubyte r, GLubyte g, GLubyte b, short * h, GLubyte * s, GLubyte * v);
Image_HSV conv_RGB_img_to_HSV_img(Image src);
void conv_RGB_HSV(Image src, Image_HSV dst);
Image_HSV_compressed create_compressed_image_from_HSV(Image_HSV img);
void compress_H(Image_HSV img, Image_HSV_compressed *dst);
void compress_SV(Image_HSV img, Image_HSV_compressed * dst, int type);
Image decompress_RGB(Image_RGB_compressed img);

#endif