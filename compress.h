#ifndef COMPRESS_H
#define COMPRESS_H
#include "ima.h"


#define RED 0
#define GREEN 1
#define BLUE 2

struct Image_compressed
{
    unsigned long sizeX;
    unsigned long sizeY;
    GLbyte *R;
    GLbyte *G;
    GLbyte *B;
    
};
typedef struct Image_compressed Image_compressed;

void compress_loop(Image img, Image_compressed dst,unsigned long sizeX, unsigned long sizeY, int begin_step);
GLbyte * reduce_raw_compressed(GLbyte* raw_compressed, unsigned long *size);


void compress(Image img);


void print_image(Image img);
#endif