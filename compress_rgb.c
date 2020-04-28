#include "compress.h"




/**
 * @brief compress a array of GLubytes and save them in storage
 * 
 * @param data array to be compressed
 * @param storage array of result
 * @param type type of the GLubytes, it's just the start index of the for loop, some macro are defined in compress.h
 * @param size size of the data array 
 * @param img_step step taken by the for loop at each iteration, some macro are defined in compress.h
 * @return unsigned long k, return the size of blocs of the compressed data
 */
unsigned long compress_GLubyte(GLubyte * data, GLbyte * storage, int type, unsigned long size, unsigned long img_step){
    GLubyte buffer = data[type];
    GLbyte counter =0;
    unsigned long k = 0;
    for (unsigned long i = type; i < size * img_step; i+=img_step){
        if(buffer == data[i]){
            if(counter < 127){
                counter++;
            }else{
                storage[k++] = counter; 
                storage[k++] = buffer;
                counter = 1;
            }
        }else{
            storage[k++] = counter; 
            storage[k++] = buffer;
            counter = 1;
            buffer = data[i];
        }
    }
    storage[k++] = counter; 
    storage[k++] = buffer;
    return k;
}


/**
 * @brief compress a specific data channel of a RGB image and save it in the dst image
 * 
 * @param img the source image
 * @param dst the destination image
 * @param color the given color channel (RGB), or use macro in compress.h
 * @return unsigned long k, the size of the compressed data field
 */
unsigned long compress_RGB(Image img, Image_RGB_compressed* dst, int color){
    //brut compression, thefore, the biggest increase of memory is x2 (case where every pixel are different)
    GLbyte * tmp_storage = malloc( img.sizeX * img.sizeY * 2 * sizeof(GLbyte));
    assert(tmp_storage);
    unsigned long k = compress_GLubyte(img.data, tmp_storage, color, img.sizeX * img.sizeY, STEP_RGB);
    
    // print out the result of the first brut compression
    /*
    printf("\n>>>size first compression = %ld\n[ ", k);
    for (size_t i = 0; i < k; i++){printf("%hhu ",tmp_storage[i]);}
    printf(" ]\n  ========================= \n");
    */
        

    //reduce the compressed data with SGI method
    tmp_storage = reduce_raw_compressed(tmp_storage, &k);
    dst->data[color] = (GLubyte*)tmp_storage; 
    return k;
}

/**
 * @brief reduce the brut compressed version of GLbytes with SGI method
 * 
 * @param raw_compressed the array of first compression 
 * @param size the size of the given array
 * @return GLbyte* 
 */
GLbyte * reduce_raw_compressed(GLbyte* raw_compressed, unsigned long * size){
    
    GLbyte * result = malloc( *size * sizeof(GLbyte));
    assert(result);
    result[0] = 0;

    unsigned long empty_pt = 0, index_pt = 0;
    //printf("start compressing\n");
    for (unsigned long i = 0; i < *size; i+=2)//iterate on the cases that contains index counter
    {   
        if(raw_compressed[i] > maximum_repeat){
        // simple case, we got a high repetition counter, simply save to the result
            //printf("large i=%ld\n",i);
            index_pt = empty_pt++; // index_pt take the empty emplacement, and empty_pt move to the next empty case
            result[index_pt] = raw_compressed[i];// save the index counter in result
            result[empty_pt++] = raw_compressed[i+1];//sauve the value in result
        }
        else if(result[index_pt] < 0 && (result[index_pt] - raw_compressed[i] >= -127)){
        // second case, an reduction is occuring, also, we can continue to reduce 
            //printf("%hhi %hhi %hhi \n",result[index_pt] , raw_compressed[i], result[index_pt] - raw_compressed[i]);
            result[index_pt] -= raw_compressed[i]; //increase the index counter 
            for (unsigned long j = 0; j < raw_compressed[i]; j++){//add the amount of value at the tail of result
                result[empty_pt++] = raw_compressed[i+1];
            }    
        }else{// last case, a new index counter needs to be saved
            //printf("new i=%ld \n",i);
            index_pt = empty_pt++; // index_pt take the empty emplacement, and empty_pt move to the next empty case
            result[index_pt] = raw_compressed[i] * -1; // save the negative index counter in result
            for (unsigned long j = 0; j < raw_compressed[i]; j++){//add the amount of value at the tail of result
                result[empty_pt++] = raw_compressed[i+1];
            }
        }

    }
    
    // print out the reduced result in the console
    /*
    printf("After compression : empty_pt = %ld\n[", empty_pt);
    //printf("After compression : index_pt = %ld\n[ ", index_pt);
    for (unsigned long i = 0; i < empty_pt; i++){printf("%hhi ", result[i]);}
    printf("]\n ****************************************\n");
    */

    free(raw_compressed);
    *size = empty_pt; 
    return result;

}


/**
 * @brief Create a compressed image from RGB object
 * 
 * @param img 
 * @param result 
 * @return int return 1 if succes
 */
int create_compressed_image_from_RGB(Image *img, Image_RGB_compressed *result){
    result->sizeX = img->sizeX;
    result->sizeY = img->sizeY;
    result->data = malloc( 3 * sizeof( unsigned long) );
    assert(result->data);
    result->ChannelSize = malloc( 3 * sizeof( GLubyte *));
    assert(result->ChannelSize);
    result->ChannelSize[RED] = compress_RGB(*img, result, RED);
    result->ChannelSize[GREEN] = compress_RGB(*img, result, GREEN);
    result->ChannelSize[BLUE] = compress_RGB(*img, result, BLUE);
    //printf_compressed_img(*result);
    return 1;
}

/**
 * @brief print out in the console the RGB compressed image, not suitable for large file, debug use only
 * 
 * @param img the given image
 */
void printf_compressed_img(Image_RGB_compressed img){
    
    printf("%hhi ", img.data[0][0]);
    for (size_t i = 1; i < 100; i++)
    {
        printf("%hhu ", img.data[0][i]);
    }

    printf("\n%hhi ", img.data[1][0]);
    for (size_t i = 1; i < 100; i++)
    {
        printf("%hhu ", img.data[1][i]);
    }

    printf("\n%hhi ", img.data[2][0]);
    for (size_t i = 1; i < 100; i++)
    {
        printf("%hhu ", img.data[2][i]);
    }
    
}
