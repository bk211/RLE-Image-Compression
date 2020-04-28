#include "compress.h"

/**
 * @brief decompress the compressed GLubyte array to destination array
 * 
 * @param src source compressed array
 * @param dst destination array
 * @param size_src size of the source array
 * @param pos sub-position in the pixel
 * @param coeff coefficient to apply to get the real position of the pixel
 */
void decompress_GLubytes(GLubyte * src, GLubyte * dst, unsigned long size_src, int pos, int coeff){
    unsigned long i = 0;
    GLbyte iter_buffer;
    GLubyte value_buffer;
    unsigned long size_counter = 0;
    while (i < size_src){
        iter_buffer = src[i];
        //printf("iter_buffer = %hhi\n", iter_buffer);
        if(iter_buffer > 0){//simple high repetition case 
            //printf("positive case\n");
            value_buffer = src[i+1];
            for (size_t k = 0; k < iter_buffer; k++){
                //printf("value_buffer = %hhu ,pos = %ld\n",value_buffer,size_counter * coeff + pos );
                dst[size_counter++ * coeff + pos] = value_buffer;
            }
            i+=2;

        }else{ //negative repetition case
            //printf("negative case\n");
            iter_buffer = iter_buffer * -1;

            for (size_t k = 0; k < iter_buffer; k++){
                value_buffer = src[++i];
                //printf("value_buffer = %hhu ,pos = %ld\n",value_buffer,size_counter * coeff + pos );
                dst[size_counter++ * coeff + pos] = value_buffer;
            }
            i++;               
        }
    }
}

/**
 * @brief decompress the RGB compressed image to RGB image
 * 
 * @param img source 
 * @param result destination
 */
void decompress_RGB(Image_RGB_compressed *img, Image * result){
    result->sizeX = img->sizeX;
    result->sizeY = img->sizeY;
    //printf(">channels size: %lu %lu %lu\n", img->ChannelSize[RED], img->ChannelSize[GREEN], img->ChannelSize[BLUE]);
    result->data = malloc( result->sizeX * result->sizeY * 3 * sizeof(GLubyte));
    assert(result->data);

    for (int i = 0; i < 3; i++){
        decompress_GLubytes(img->data[i], result->data, img->ChannelSize[i], i, 3);    
    }
}

/**
 * @brief convert a HSV image to RGB image
 * 
 * @param src ptr to source HSV image
 * @param dst ptr to destination RGB image
 * @param size dimension of the image source
 */
void conv_HSV_RGB(Image_HSV *src, Image * dst, unsigned long size){
    GLubyte r,g,b;
    /*
        printf("in conv hsv rgb\n");
        printf("%hi %hhi %hhi\n",src->Hdata[0], src->SVdata[0][0], src->SVdata[1][0]);
         for (size_t i = 1; i < 5; i++)
        {
        printf("%hi %hhu %hhu\n", src->Hdata[i], src->SVdata[0][i], src->SVdata[1][i]);
    }*/

    for (size_t i = 0; i < size; i++){
        hsv_to_rgb(src->Hdata[i], src->SVdata[S][i], src->SVdata[V][i], &r, &g, &b);
        dst->data[i*3] = r;
        dst->data[i*3 +1] = g;
        dst->data[i*3 +2] = b;
    }
    
}


/**
 * @brief convert a HSV image to RGB image
 * 
 * @param src ptr to source HSV image
 * @param result ptr to destination RGB image
 */
void conv_HSV_img_to_RGB_img(Image_HSV *src, Image *result){
    result->sizeX = src->sizeX;
    result->sizeY = src->sizeY;
    unsigned long size = result->sizeX * result->sizeY;
    result->data = malloc( 3 * size * sizeof(result->data));    
    assert(result->data);
    conv_HSV_RGB(src, result, size);
    
}
 

/**
 * @brief decompress a HSV compressed image to a HSV image
 * 
 * @param img source image
 * @param result destination image
 */
void decompress_HSV(Image_HSV_compressed *img, Image_HSV * result){
    result->sizeX = img->sizeX;
    result->sizeY = img->sizeY;
    unsigned long size = result->sizeX * result->sizeY;
    
    result->SVdata = malloc(2 * sizeof(GLubyte*));
    assert(result->SVdata);
    result->SVdata[S] = malloc( size * sizeof(GLubyte));
    assert(result->SVdata[S]);
    result->SVdata[V] = malloc( size * sizeof(GLubyte));
    assert(result->SVdata[V]);
    result->Hdata = malloc( size * sizeof(GLshort));
    assert(result->Hdata);
    for (size_t i = 0; i < 2; i++){
        decompress_GLubytes(img->SVdata[i], result->SVdata[i], img->ChannelSize[i], 0, 1);
    }
    decompress_GLshort(img->Hdata, result->Hdata, img->ChannelSize[H]);
}


/**
 * @brief decompress the compressed GLshort array to destination array
 * 
 * @param src source compressed array
 * @param dst destination array
 * @param size_src size of the source array
 */
void decompress_GLshort(GLshort * src, GLshort * dst, unsigned long size_src){
    unsigned long j = 0;
    GLshort iter_buffer;
    GLshort value_buffer;
    unsigned long ptr_empty = 0;
    while (j < size_src){
        iter_buffer = src[j];
        if(iter_buffer > 0){
            value_buffer = src[j+1];
            for (size_t k = 0; k < iter_buffer; k++){
                dst[ptr_empty++ ] = value_buffer;
            }
            j+=2;

        }else{
            iter_buffer = iter_buffer * -1;

            for (size_t k = 0; k < iter_buffer; k++){
                value_buffer = src[++j];
                dst[ptr_empty++ ] = value_buffer;
            }
            j++;
        }
    }
}



/**
 * @brief convert 3 hsv values to rgb values
 * 
 * @param h hue 
 * @param s saturation
 * @param v value
 * @param r red
 * @param g green
 * @param b blue
 */
void hsv_to_rgb(short h, GLubyte s, GLubyte v, GLubyte * r, GLubyte * g, GLubyte * b){
    //printf("\nh = %hi, s = %hhu, v = %hhu\n", h,s,v);
    double _S = s / 100.0;
    double _V = v / 100.0;
    double C = _V * _S;
    double M = _V - C ;
    double X = C * (1 - fabs( fmod((h/60.0), 2) -1 ));
    //printf("X = %f\n", X);
    //printf("S = %f  V= %f  C= %f M = %f\n",_S, _V, C, M);

    double _R, _G, _B;
    if(h >= 0 && h < 60){
        _R = C;
        _G = X;
        _B = 0;
    }else if(h < 120){
        _R = X;
        _G = C;
        _B = 0;
    }else if(h < 180){
        _R = 0;
        _G = C;
        _B = X;
    }else if(h < 240){
        _R = 0;
        _G = X;
        _B = C;
    }else if(h < 300){
        _R = X;
        _G = 0;
        _B = C;
    }else if(h < 360){
        _R = C;
        _G = 0;
        _B = X;
    }else{
        _R = 0;
        _G = 0;
        _B = 0;
    }

    //printf("G =%f\n", (_G+M) * 255);
    *r = (_R + M) * 255 + 0.5;
    *g = (_G + M) * 255 + 0.5;
    *b = (_B + M) * 255 + 0.5;
    //printf("results: r= %hhu, g=%hhu, b=%hhu", *r, *g, *b);
}
