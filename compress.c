#include "ima.h"
Image gen_test_image(){
    Image foo;
    foo.sizeX = 5;
    foo.sizeY = 5;
    foo.data = malloc( 10*10 *sizeof(foo.sizeX));
    for (size_t i = 0; i < 75; i++)
    {

        if(i >= 45){
            foo.data[i] = 80;
        }else{
            foo.data[i] = 50;
        }
    }
    
    
    return foo;
}

void print_image(Image img){
    for (size_t i = 0; i < img.sizeY; i++)
    {
        for (size_t j = 0; j < img.sizeX*3; j++)
        {
            printf("%hhi ",img.data[i*img.sizeY*3 + j]);
        }
        printf("\n");
    }
    
}

void compress_loop(Image img, unsigned long sizeX, unsigned long sizeY, int range){
    
}

void compress(Image img){
    unsigned long sizeX = img.sizeX;
    unsigned long sizeY = img.sizeY;


}