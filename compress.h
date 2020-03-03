#ifndef COMPRESS_H
#define COMPRESS_H
#include "ima.h"

struct Image_compressed
{
    unsigned long sizeX;
    unsigned long sizeY;
    GLubyte *R;
    GLubyte *G;
    GLubyte *B;
    
};



Image gen_test_image();
void compress(Image img);


void print_image(Image img);
#endif