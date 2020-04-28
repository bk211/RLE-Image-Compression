#ifndef COMPRESS_H
#define COMPRESS_H
#include "ima.h"
#include <math.h>

#define RED 0
#define GREEN 1
#define BLUE 2
#define S 0
#define V 1
#define H 2
#define SV 0
#define maximum_repeat 2
#define STEP_RGB 3
#define STEP_HSV 1

/**
 * @brief struct of compressed image in RGB format
 * 
 */
struct Image_RGB_compressed
{
    unsigned long sizeX;
    unsigned long sizeY;
    unsigned long * ChannelSize;
    GLubyte **data;
};
typedef struct Image_RGB_compressed Image_RGB_compressed;

/**
 * @brief struct of image in HSV format
 * 
 */
struct Image_HSV
{
    unsigned long sizeX;
    unsigned long sizeY;
    GLshort * Hdata;
    GLubyte **SVdata;
};
typedef struct Image_HSV Image_HSV;

/**
 * @brief struct of compressed image in HSV format
 * 
 */
struct Image_HSV_compressed
{
    unsigned long sizeX;
    unsigned long sizeY;
    unsigned long * ChannelSize;
    GLshort * Hdata;
    GLubyte **SVdata;
};
typedef struct Image_HSV_compressed Image_HSV_compressed;


void print_image(Image img);
unsigned long compress_GLubyte(GLubyte * data, GLbyte * storage, int type, unsigned long size, unsigned long img_step);
unsigned long compress_RGB(Image img, Image_RGB_compressed * dst, int color);
GLbyte * reduce_raw_compressed(GLbyte* raw_compressed, unsigned long *size);
double max3(double x, double y, double z);
double min3(double x, double y, double z);
void rgb_to_hsv(GLubyte r, GLubyte g, GLubyte b, short * h, GLubyte * s, GLubyte * v);
void conv_RGB_HSV(Image *src, Image_HSV * dst, unsigned long size);
void conv_RGB_img_to_HSV_img(Image *src, Image_HSV *result);
int create_compressed_image_from_RGB(Image *img, Image_RGB_compressed *result);
GLshort * reduce_raw_compressed_hue(GLshort* raw_compressed, unsigned long * size);
unsigned long compress_GLshort(GLshort * data, GLshort * storage, unsigned long size);
void compress_SV(Image_HSV * img, Image_HSV_compressed * dst, int type);
void compress_H(Image_HSV * img, Image_HSV_compressed *dst);
void create_compressed_image_from_HSV(Image_HSV *img , Image_HSV_compressed *result);
void decompress_GLubytes(GLubyte * src, GLubyte * dst, unsigned long size_src, int pos, int coeff);
void decompress_RGB(Image_RGB_compressed *img, Image * result);
void save_compressed_RGB_image(char * filename,Image_RGB_compressed * img);
void save_compressed_HSV_image(char * filename, Image_HSV_compressed * img);
int Image_load(char *filename, Image *img);
void hsv_to_rgb(short h, GLubyte s, GLubyte v, GLubyte * r, GLubyte * g, GLubyte * b);
void conv_HSV_RGB(Image_HSV *src, Image * dst, unsigned long size);
void conv_HSV_img_to_RGB_img(Image_HSV *src, Image *result);
void decompress_HSV(Image_HSV_compressed *img, Image_HSV * result);
void decompress_GLshort(GLshort * src, GLshort * dst, unsigned long size_src);
void printf_compressed_img(Image_RGB_compressed img);
void free_images(Image * img1, Image_RGB_compressed * img2, Image_HSV * img3, Image_HSV_compressed * img4);

#endif