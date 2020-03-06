#include "compress.h"



void print_image(Image img){
    for (size_t i = 0; i < img.sizeY; i++)
    {
        for (size_t j = 0; j < img.sizeX; j++)
        {
            printf("[%hhi,%hhi,%hhi]",img.data[i*img.sizeY*3 + j*3],img.data[i*img.sizeY*3 + j*3+1],img.data[i*img.sizeY*3 + j *3 +2]);
        }
        printf("\n");
    }
    
}



void compress_loop(Image img, Image_compressed dst,unsigned long sizeX, unsigned long sizeY, int begin_step){
    
    GLubyte buffer = img.data[begin_step];
    GLbyte counter =0;
    GLbyte * tmp_storage = malloc( sizeX*sizeY *3* sizeof(GLubyte)); 
    unsigned long k = 0;

    for (unsigned long i = begin_step; i < sizeX * sizeY * 3; i+=3)
    {
        printf("%hhi, ", img.data[i]);
        if(buffer == img.data[i]){//si suite de valeur identique
            counter++;
        //    printf(">i=%ld : same\n", i);
        }else{ // sinon 
        //    printf(">i=%ld : not same\n", i);
            tmp_storage[k++] = counter; 
            tmp_storage[k++] = buffer;
            counter = 1;
            buffer = img.data[i];
        }
    }
    tmp_storage[k++] = counter; 
    tmp_storage[k++] = buffer;


    // affiche le contenue de tmp_storage
    printf("\n>>>k = %ld\n[ ", k);
    for (size_t i = 0; i < k; i++)
    {
        printf("%hhi ",tmp_storage[i]);
    }
    printf(" ]\n");
}

void compress(Image img){
    unsigned long sizeX = img.sizeX;
    unsigned long sizeY = img.sizeY;



}