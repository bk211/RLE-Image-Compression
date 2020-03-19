#ifndef COMPRESS_H
#define COMPRESS_H
#include "ima.h"
#include <math.h>

#define RED 0
#define GREEN 1
#define BLUE 2

struct Image_compressed
{
    unsigned long sizeX;
    unsigned long sizeY;
    unsigned long * sizeChannel;
    GLbyte **data;
};
typedef struct Image_compressed Image_compressed;

unsigned long compress_loop(Image img, Image_compressed * dst,unsigned long sizeX, unsigned long sizeY, int color);
GLbyte * reduce_raw_compressed(GLbyte* raw_compressed, unsigned long *size);


Image_compressed compress(Image img);

void save_compressed_image(char * filename,Image_compressed * img);


void print_image(Image img);
void rgb_to_hsv(GLubyte r, GLubyte g, GLubyte b, short * h, GLubyte * s, GLubyte * v);

#endif