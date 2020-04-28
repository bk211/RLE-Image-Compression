#include "compress.h"

/**
 * @brief return the maximum of 3 given parameter
 * 
 * @param x value
 * @param y value
 * @param z value
 * @return double greatest of xyz
 */
double max3(double x, double y, double z){
    return x > y? (x > z? x : z) : (y > z ? y : z);
}

/**
 * @brief return the minimum of 3 given parameter
 * 
 * @param x value
 * @param y value
 * @param z value
 * @return double smallest of xyz
 */
double min3(double x, double y, double z){
    return x < y? (x < z? x : z) : (y < z ? y : z);
}

/**
 * @brief convert 3 RGB values to HSV values
 * 
 * @param r red field
 * @param g green field
 * @param b blue field
 * @param h hue
 * @param s saturation
 * @param v value
 */
void rgb_to_hsv(GLubyte r, GLubyte g, GLubyte b, short * h, GLubyte * s, GLubyte * v){
    //printf("\nR = %hhu, G = %hhu, B = %hhu\n", r,g,b);
    double R = r / 255.0 , G = g / 255.0 , B = b / 255.0;
    //printf("R = %lf, G = %lf, B = %lf\n", R, G, B);
    double M = max3(R, G, B);
    double m = min3(R, G, B);
    double C = M - m;
    //printf("M = %lf, m = %lf, C = %lf \n", M, m, C);
    
    if(C == 0 ){// cas indefinie
        *h = 361;
        //printf("c0, %hi",*h);
    }else if(M == R){
        *h = round(fmod(((G-B) / C ) , 6) * 60);
        *h = *h > 0? *h : *h + 360; 
        //printf("R*h = %hi \n",*h);
    }else if(M == G){
        *h = round(fmod(((B-R) / C + 2) , 6) * 60);
        *h = *h > 0? *h : *h + 360; 
        //printf("GT = %hi \n",*h);    
    }else if(M == B){
        *h = round(fmod(((R-G) / C + 4), 6.0) * 60);
        *h = *h > 0? *h : *h + 360; 
        //printf("BT = %hi \n",*h);    
    }
    
    if(M == 0 ){
        *s = 0;
    }else{
        *s = (C / M) * 100 + 0.5;
    }
    
    *v = M * 100 + 0.5;
    //printf("result >> %hi %hhu %hhu\n", *h, *s, *v);
    
}

/**
 * @brief convert a RGB image to HSV image, dst->data is considered as malloced.
 * 
 * @param src ptr to source RGB image
 * @param dst ptr to destination HSV image
 * @param size dimension of the image source
 */
void conv_RGB_HSV(Image *src, Image_HSV * dst, unsigned long size){
    short h;
    GLubyte s,v;
    for (size_t i = 0; i < size; i++){
        rgb_to_hsv(src->data[i*3], src->data[i*3+1], src->data[i*3+2], &h, &s, &v);
        dst->Hdata[i] = h;
        dst->SVdata[S][i] = s;
        dst->SVdata[V][i] = v;
    }
}


/**
 * @brief convert a RGB image to HSV image, malloc the necessary memory to dst
 * 
 * @param src ptr to source RGB image
 * @param result ptr to destination HSV image
 */
void conv_RGB_img_to_HSV_img(Image *src, Image_HSV *result){
    result->sizeX = src->sizeX;
    result->sizeY = src->sizeY;
    unsigned long size = result->sizeX * result->sizeY;
    result->Hdata = malloc( size * sizeof(result->Hdata));
    assert(result->Hdata);
    result->SVdata = malloc( 2 * sizeof(result->SVdata));    
    assert(result->SVdata);
    result->SVdata[S] = malloc( size * sizeof(result->SVdata[S]));
    assert(result->SVdata[S]);
    result->SVdata[V] = malloc( size * sizeof(result->SVdata[V]));
    assert(result->SVdata[V]);
    conv_RGB_HSV(src, result, size);
    
}

/**
 * @brief reduce the compressed Hue field with SGI method, for commented version, jump to reduce_raw_compressed function
 * 
 * @param raw_compressed the hue field after first compression
 * @param size size of the hue field
 * @return GLshort* return the result hue field
 */
GLshort * reduce_raw_compressed_hue(GLshort* raw_compressed, unsigned long * size){
    
    GLshort * result = malloc( *size * sizeof(GLshort));
    assert(result);
    result[0] = 0;

    unsigned long empty_pt = 0, index_pt = 0;
    for (unsigned long i = 0; i < *size; i+=2)
    {
        if(raw_compressed[i] > maximum_repeat){ 
            index_pt = empty_pt++;
            result[index_pt] = raw_compressed[i];
            result[empty_pt++] = raw_compressed[i+1];
        }
        else if(result[index_pt] < 0 && (result[index_pt] - raw_compressed[i] >= -32767)){
            result[index_pt] -= raw_compressed[i];
            for (unsigned long j = 0; j < raw_compressed[i]; j++){
                result[empty_pt++] = raw_compressed[i+1];
            }
            
        }else{
            index_pt = empty_pt++;
            result[index_pt] = raw_compressed[i] * -1;
            for (unsigned long j = 0; j < raw_compressed[i]; j++){
                result[empty_pt++] = raw_compressed[i+1];
            }
        }

    }
    
    // print out the result of the reduction
    /*printf("After compression : empty_pt = %ld\n[", empty_pt);
    printf("After compression : index_pt = %ld\n[ ", index_pt);
    for (unsigned long i = 0; i < empty_pt; i++)
        {printf("%hi ", result[i]);}
    printf(" ]\n ****************************************\n");
    */
    
    free(raw_compressed);
    *size = empty_pt; 
    return result;

} 

/**
 * @brief compress a array of GLshort and save them in storage, just a GLshort version of compress_GLubyte 
 * 
 * @param data array to be compressed
 * @param storage array of result
 * @param size size of the data array 
 * @return unsigned long k, return the size of blocs of the compressed data
 */
unsigned long compress_GLshort(GLshort * data, GLshort * storage, unsigned long size){
    GLshort counter =0;
    GLushort buffer = data[0];
    unsigned long k = 0;
    for (unsigned long i = 0; i < size; i++)
        {
            if(buffer == data[i]){
                if(counter < 32767){
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
 * @brief compress the saturation and value filed from source to destinamtion 
 * 
 * @param img source image
 * @param dst destination image
 * @param type the field type (S or V)
 */
void compress_SV(Image_HSV *img, Image_HSV_compressed *dst, int type){
    GLbyte * tmp_storage = malloc( img->sizeX * img->sizeY * 2 * sizeof(tmp_storage)); // compression brute, la pire cas possible est un dedoublement de memoire
    assert(tmp_storage);
    unsigned long k = compress_GLubyte(img->SVdata[type], tmp_storage, SV, img->sizeX * img->sizeY, STEP_HSV);

    tmp_storage = reduce_raw_compressed(tmp_storage, &k);
    dst->SVdata[type] = (GLubyte*)tmp_storage;
    dst->ChannelSize[type] = k;
}

/**
 * @brief compress the hue field from source to destination
 * 
 * @param img source image
 * @param dst destination image
 */
void compress_H(Image_HSV * img, Image_HSV_compressed *dst){
    GLshort * tmp_storage = malloc( img->sizeX * img->sizeY * 2 * sizeof(GLshort));
    assert(tmp_storage);
    unsigned long k = compress_GLshort(img->Hdata, tmp_storage, img->sizeX * img->sizeY);
    tmp_storage = reduce_raw_compressed_hue(tmp_storage,&k);
    dst->Hdata = tmp_storage;
    dst->ChannelSize[H] = k;
}

/**
 * @brief Create a compressed image in HSV format from HSV image
 * 
 * @param img the given Image_HSV image
 * @param result Image_HSV_compressed result
 */
void create_compressed_image_from_HSV(Image_HSV *img , Image_HSV_compressed *result){
    result->sizeX = img->sizeX;
    result->sizeY = img->sizeY;
    result->ChannelSize = malloc(3 * sizeof(unsigned long));
    assert(result->ChannelSize);
    result->SVdata = malloc(2 * sizeof(GLubyte *));
    assert(result->SVdata);
    compress_H(img, result);
    compress_SV(img, result, S);
    compress_SV(img, result, V);
}