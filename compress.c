#include "compress.h"
// maximum of repetition allowed for reduction function
const unsigned long maximum_repeat = 2;

/**
 * @brief print out the image in the console, not suitable for large file
 * 
 * @param img 
 */
void print_image(Image img){
    for (size_t i = 0; i < img.sizeY; i++)
    {
        for (size_t j = 0; j < img.sizeX; j++)
        {
            printf("%hhu %hhu %hhu\n",img.data[i*img.sizeY*3 + j*3],img.data[i*img.sizeY*3 + j*3+1],img.data[i*img.sizeY*3 + j *3 +2]);
        }
        printf("\n");
    }
    
}

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
    unsigned long j = 0;
    GLbyte iter_buffer;
    GLubyte value_buffer;
    unsigned long size_counter = 0;
    while (j < size_src){
        iter_buffer = src[j];
        //printf("iter_buffer = %hhi\n", iter_buffer);
        if(iter_buffer > 0){//simple high repetition case 
            //printf("positive case\n");
            value_buffer = src[j+1];
            for (size_t k = 0; k < iter_buffer; k++){
                //printf("value_buffer = %hhu ,pos = %ld\n",value_buffer,size_counter * coeff + pos );
                dst[size_counter++ * coeff + pos] = value_buffer;
            }
            j+=2;

        }else{ //negative repetition case
            //printf("negative case\n");
            iter_buffer = iter_buffer * -1;

            for (size_t k = 0; k < iter_buffer; k++){
                value_buffer = src[++j];
                //printf("value_buffer = %hhu ,pos = %ld\n",value_buffer,size_counter * coeff + pos );
                dst[size_counter++ * coeff + pos] = value_buffer;
            }
            j++;               
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
 * @brief save the RGB compressed image to a file given the filename
 * 
 * @param filename filename
 * @param img source image
 */
void save_compressed_RGB_image(char * filename, Image_RGB_compressed * img){
    FILE *fp;
    unsigned long c;
    //open file for output
    fp = fopen(filename, "wb");
    if (!fp) {
         fprintf(stderr, "Unable to open file '%s'\n", filename);
         exit(1);
    }

    //write the header file
    //image format
    fprintf(fp, "P7\n");

    //comments
    fprintf(fp, "# Created by %s\n","CC");

    //image size
    fprintf(fp, "%lu %lu\n",img->sizeX,img->sizeY);

    // rgb component depth
    fprintf(fp, "%d\n",255);
    //fprintf(fp, "%d\n",RGB_COMPONENT_COLOR);

    // channelSize 
    fprintf(fp, "%lu %lu %lu\n", img->ChannelSize[RED], img->ChannelSize[GREEN], img->ChannelSize[BLUE]);
    
    // pixel data
    for (size_t i = 0; i < 3; i++){
        if( (c = fwrite(img->data[i], (size_t) 1, (size_t) img->ChannelSize[i], fp)) != img->ChannelSize[i]){
            fprintf(stderr, "Failed to write data\n");
            exit(1);
        }
        printf("wrote c : %ld  | expected : %ld\n", c, img->ChannelSize[i]);
    }

    fclose(fp);
}

/**
 * @brief save the HSV compressed image to a file given the filename
 * 
 * @param filename filename
 * @param img source image
 */
void save_compressed_HSV_image(char * filename, Image_HSV_compressed * img){
    FILE *fp;
    //open file for output
    fp = fopen(filename, "wb");
    if (!fp) {
         fprintf(stderr, "Unable to open file '%s'\n", filename);
         exit(1);
    }

    //write the header file
    //image format
    fprintf(fp, "P8\n");

    //comments
    fprintf(fp, "# Created by %s\n","CC");

    //image size
    fprintf(fp, "%lu %lu\n",img->sizeX,img->sizeY);

    // rgb component depth
    fprintf(fp, "%d\n",255);
    //fprintf(fp, "%d\n",RGB_COMPONENT_COLOR);

    unsigned long c;
    // channelSize // pixel data
    fprintf(fp, "%lu %lu %lu\n", img->ChannelSize[S], img->ChannelSize[V], img->ChannelSize[H]);
    
    if((c = fwrite(img->SVdata[S], (size_t) 1, (size_t) img->ChannelSize[S], fp)) != img->ChannelSize[S]){
        fprintf(stderr, "Failed to write data\n");
        exit(1);
    }
    printf("wrote S : %ld  | expected : %ld\n", c, img->ChannelSize[S]);
    
    if((c = fwrite(img->SVdata[V], (size_t) 1, (size_t) img->ChannelSize[V], fp)) != img->ChannelSize[V]){
        fprintf(stderr, "Failed to write data\n");
        exit(1);
    }
    
    printf("wrote V : %ld  | expected : %ld\n", c, img->ChannelSize[V]);
    if((c = fwrite(img->Hdata, (size_t) sizeof(GLshort), (size_t) img->ChannelSize[H], fp)) != img->ChannelSize[H]){
        fprintf(stderr, "Failed to write data\n");
        exit(1);
    }
    printf("wrote H : %ld  | expected : %ld\n", c, img->ChannelSize[H]);
    fclose(fp);
}

/**
 * @brief load a given image file 
 * 
 * @param filename filename
 * @param img destination image
 * @return succes code otherwise exit(1) the entire program
 */
int Image_load(char *filename, Image *img){
    char d, buff[16];
    FILE *fp;
    int b, c, rgb_comp_color, size, sizex, sizey;
	GLubyte tmp, * ptrdeb, *ptrfin, *lastline;
    unsigned long r;
    //open PPM file for reading
    fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        exit(1);
    }

    //read image format
    if (!fgets(buff, sizeof(buff), fp)) {
        perror(filename);
        exit(1);
    }

    //check the image format
    if (buff[0] != 'P' || ( buff[1] != '6' && buff[1] != '7' && buff[1] != '8')) {
         fprintf(stderr, "Invalid image format (must be 'P6' or 'P7' or 'P8)\n");
         exit(1);
    }

    //check for comments
    c = getc(fp);
    while (c == '#') {
	  while (getc(fp) != '\n')
		;
	  c = getc(fp);
    }
    ungetc(c, fp);
    
    //read image size information
    if (fscanf(fp, "%lu %lu", &img->sizeX, &img->sizeY) != 2) {
         fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
         exit(1);
    }

    //read rgb component
    if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
         fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
         exit(1);
    }

    fscanf(fp, "%c ", &d);
    //check rgb component depth
    if (rgb_comp_color!= 255) {
         fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
         exit(1);
    }

	/* allocation memoire */
	size = img->sizeX * img->sizeY * 3;
	img->data = (GLubyte *) malloc ((size_t) size * sizeof (GLubyte));
	assert(img->data);

    if(buff[1] == '6'){
        //read pixel data from file
        if (fread(img->data, (size_t) 1, (size_t) size, fp) == 0) {
             fprintf(stderr, "Error loading image '%s'\n", filename);
	    	 /*
             exit(1);
	    	 */
        }
        /* remettre l image dans le bon sens */
	    //	printf("debut %ld fin %ld\n", (int) img->data, (int) img->data + size);
	    sizex = img->sizeX;
	    sizey = img->sizeY;
	    lastline = img->data + size - sizex * 3;
	    for (b = 0; b < sizey / 2; b++) {
	        ptrdeb = img->data + b * sizex * 3;
	        ptrfin = lastline - (b * sizex * 3);
	    //	printf("%d => %ld %ld\n", b, (int) ptrdeb, (int) ptrfin);
	      for (c = 0; c < 3 * sizex; c++) {
	    	tmp = *ptrdeb;
	    	*ptrdeb = *ptrfin;
	    	*ptrfin = tmp;
	    	ptrfin++;
	    	ptrdeb++;
	        }		
	    }

    }else if(buff[1] == '7'){
        Image_RGB_compressed img_comp;
        img_comp.sizeX = img->sizeX;
        img_comp.sizeY = img->sizeY;
        img_comp.ChannelSize = malloc(3 * sizeof(unsigned long));
        img_comp.data = malloc(3 * sizeof(GLubyte*));

        //printf("size: %lu %lu\n", img_comp.sizeX, img_comp.sizeY);
        if (fscanf(fp, "%lu %lu %lu\n", &img_comp.ChannelSize[RED], &img_comp.ChannelSize[GREEN], &img_comp.ChannelSize[BLUE]) != 3){
            fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
            exit(1);
        }
        
        for (size_t i = 0; i < 3; i++){
            img_comp.data[i] = malloc(img_comp.ChannelSize[i] * sizeof(GLubyte));
            if((r = fread(img_comp.data[i], (size_t)1, (size_t)img_comp.ChannelSize[i], fp)) != img_comp.ChannelSize[i] ){
                fprintf(stderr,"Failed to read data");
                exit(1);
            }
            printf("Blocs read = %lu | expected %lu\n",r, img_comp.ChannelSize[i]);
        }
        
        decompress_RGB(&img_comp, img);

    }else if(buff[1] == '8'){
        Image_HSV_compressed img_comp;
        img_comp.sizeX = img->sizeX;
        img_comp.sizeY = img->sizeY;
        img_comp.ChannelSize = malloc(3 * sizeof(unsigned long));
        img_comp.SVdata = malloc(2 * sizeof(GLubyte*));

        if (fscanf(fp, "%lu %lu %lu\n", &img_comp.ChannelSize[RED], &img_comp.ChannelSize[GREEN], &img_comp.ChannelSize[BLUE]) != 3){
            fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
            exit(1);
        }
        
        img_comp.SVdata[S] = malloc(img_comp.ChannelSize[S] * sizeof(GLubyte));
        img_comp.SVdata[V] = malloc(img_comp.ChannelSize[V] * sizeof(GLubyte));
        if((r = fread(img_comp.SVdata[S], (size_t) 1, (size_t) img_comp.ChannelSize[S], fp)) != img_comp.ChannelSize[S] ){
            fprintf(stderr,"Failed to read data");
            exit(1);
        }
        printf("S blocs read = %lu | expected = %lu\n", r, img_comp.ChannelSize[S]);

        if((r = fread(img_comp.SVdata[V], (size_t) 1, (size_t) img_comp.ChannelSize[V], fp)) != img_comp.ChannelSize[V] ){
            fprintf(stderr,"Failed to read data");
            exit(1);
        }
        printf("V blocs read = %lu | expected = %lu\n", r, img_comp.ChannelSize[V]);

        if((r = fread(img_comp.Hdata, (size_t) sizeof(GLshort),(size_t) img_comp.ChannelSize[H], fp)) != img_comp.ChannelSize[H] ){
            fprintf(stderr,"Failed to read data");
            exit(1);
        }

        img_comp.Hdata = malloc(img_comp.ChannelSize[H] * sizeof(GLshort));
        printf("H blocs read = %lu | expected = %lu\n", r, img_comp.ChannelSize[H]);

        Image_HSV img_hsv;
        decompress_HSV(&img_comp, &img_hsv);
        conv_HSV_img_to_RGB_img(&img_hsv, img);
    }
    fclose(fp);
    return 1;
   

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

/**
 * @brief Free all memory malloced by thos 4 types of Image given in parameter
 * 
 * @param img1 Simple RGB image
 * @param img2 RGB compressed image
 * @param img3 Simple HSV image
 * @param img4 HSV compressed image
 */
void free_images(Image * img1, Image_RGB_compressed * img2, Image_HSV * img3, Image_HSV_compressed * img4){
    if(img1 != NULL){
        free(img1->data);
        free(img1);
    }
    if(img2 != NULL){
        free(img2->ChannelSize);
        free(img2->data);
    }
    if(img3 != NULL){
        free(img3->Hdata);
        free(img3->SVdata[S]);
        free(img3->SVdata[V]);
        free(img3->SVdata);
    }
    if(img4 != NULL){
        free(img4->Hdata);
        free(img4->SVdata[S]);
        free(img4->SVdata[V]);
        free(img4->SVdata);
    }
}