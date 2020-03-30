#include "compress.h"
// maximum de repetition autorise pour une reduction SGI
const unsigned long maximum_repeat = 2;

void print_image(Image img){
    for (size_t i = 0; i < 1; i++)
    {
        for (size_t j = 0; j < 20; j++)
        {
            printf("[%hhu,%hhu,%hhu]",img.data[i*img.sizeY*3 + j*3],img.data[i*img.sizeY*3 + j*3+1],img.data[i*img.sizeY*3 + j *3 +2]);
        }
        printf("\n");
    }
    
}

unsigned long compress_GLubyte(GLubyte * data, GLbyte * storage, int type, unsigned long size, unsigned long img_type){
    GLubyte buffer = data[type];
    GLbyte counter =0;
    unsigned long k = 0;
    for (unsigned long i = type; i < size * img_type; i+=img_type)
        {
            //printf("%hhu, ", data[i]);
            if(buffer == data[i]){//si suite de valeur identique
                if(counter < 127){
                    counter++;
                }else{
                    //printf("<limit counter, push up new> ");
                    storage[k++] = counter; 
                    storage[k++] = buffer;
                    counter = 1;
                }
            //    printf(">i=%ld : same\n", i);
            }else{ // sinon 
            //    printf(">i=%ld : not same\n", i);
                //sauvegarde dans storage
                storage[k++] = counter; 
                storage[k++] = buffer;
                //reaffectation du nouveau buffer de test
                counter = 1;
                buffer = data[i];
            }
        }

        storage[k++] = counter; 
        storage[k++] = buffer;

    return k;
}

unsigned long compress_RGB(Image img, Image_RGB_compressed* dst, int color){
    
    GLbyte * tmp_storage = malloc( img.sizeX * img.sizeY * 2 * sizeof(GLbyte)); // compression brute, la pire cas possible est un dedoublement de memoire
    assert(tmp_storage);
    unsigned long k = compress_GLubyte(img.data, tmp_storage, color, img.sizeX * img.sizeY, STEP_RGB);
    
    // affiche le contenue de tmp_storage //premiere compression brute
    
    /*
    printf("\n>>>k = %ld\n[ ", k);
    for (size_t i = 0; i < k; i++){
        printf("%hhu ",tmp_storage[i]);
    }
    printf(" ]\n  ========================= \n");
    */
        

    //reduit le resultat brute via la methode SGI
    tmp_storage = reduce_raw_compressed(tmp_storage, &k);
    // stockage dans l'image dst
    dst->data[color] = (GLubyte*)tmp_storage; 

    return k;
}

GLbyte * reduce_raw_compressed(GLbyte* raw_compressed, unsigned long * size){
    
    GLbyte * result = malloc( *size * sizeof(GLbyte));
    assert(result);
    result[0] = 0;

    unsigned long empty_pt = 0, index_pt = 0;
    //printf("start compressing\n");
    for (unsigned long i = 0; i < *size; i+=2)//itere sur les indices de repetition de raw_compressed
    {   
        //printf("i = %ld v = %hhi \n",i,raw_compressed[i]);
        if(raw_compressed[i] > maximum_repeat){ // cas simple, haute repetition, ecriture simple dans result
            //printf("large i=%ld\n",i);
            index_pt = empty_pt++; // index_pt recoit un emplace vide
            result[index_pt] = raw_compressed[i];//savegarde du compteur
            result[empty_pt++] = raw_compressed[i+1];//sauvegarde de la valeur 
        }
        else if(result[index_pt] < 0 && (result[index_pt] - raw_compressed[i] > -128)){// si compteur de redution encours
            /*printf("in reduce case i=%ld\n",i);
            printf("%hhi %hhi %hhi \n",result[0], result[1] , result[2]);
            printf("%hhi %hhi %hhi \n",result[index_pt] , raw_compressed[i], result[index_pt] - raw_compressed[i]);
            */
            result[index_pt] -= raw_compressed[i];
            for (unsigned long j = 0; j < raw_compressed[i]; j++){//ajoute le nombre de valeur necessaire
                result[empty_pt++] = raw_compressed[i+1];
            }    
        }else{// nouvelle valeur a reduire
            //printf("new i=%ld \n",i);
            index_pt = empty_pt++; // index_pt recoit un emplace vide
            result[index_pt] = raw_compressed[i] * -1; //flip du compteur
            for (unsigned long j = 0; j < raw_compressed[i]; j++){//ajoute le nombre de valeur necessaire
                result[empty_pt++] = raw_compressed[i+1];
            }
        }

    }
    
    // affiche le contenue apres reduction
    
    /*
    printf("After compression : empty_pt = %ld\n[", empty_pt);
    //printf("After compression : index_pt = %ld\n[ ", index_pt);
    for (unsigned long i = 0; i < empty_pt; i++)
    {
        printf("%hhi ", result[i]);
    }
    printf("]\n ****************************************\n");

    */

    //free(raw_compressed); // libere la memoire de la compression brute
    //reduction de l'espace memoire
    //result = (GLbyte*) realloc(result, empty_pt * sizeof(GLbyte));

    *size = empty_pt; 
    return result;

}

double max3(double x, double y, double z){
    return x > y? (x > z? x : z) : (y > z ? y : z);
}

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
        //printf("c0, %hi",T);
    }else if(M == R){
        *h = round(fmod(((G-B) / C ) , 6) * 60);
        *h = *h > 0? *h : *h + 360; 
    //    printf("R*h = %hi \n",*h);
    }else if(M == G){
        *h = round(fmod(((B-R) / C + 2) , 6) * 60);
        *h = *h > 0? *h : *h + 360; 
        //printf("GT = %hi \n",*h);    
    }else if(M == B){
        *h = round(fmod(((R-G) / C + 4), 6.0) * 60);
        *h = *h > 0? *h : *h + 360; 
  //      printf("BT = %hi \n",*h);    
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
 * @brief convert a RGB image to HSV image
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
 * @brief convert a RGB image to HSV image
 * 
 * @param src ptr to source RGB image
 * @param dst ptr to destination HSV image
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


int create_compressed_image_from_RGB(Image *img, Image_RGB_compressed *result){
    result->sizeX = img->sizeX;
    result->sizeY = img->sizeY;
    result->data = malloc( 3 * sizeof( result->data) ); // 3 ptr vers 3 tableau de donner non alloue
    assert(result->data);
    result->ChannelSize = malloc( 3 * sizeof( result->ChannelSize));// tableau de 3 size rgb
    assert(result->ChannelSize);
    result->ChannelSize[RED] = compress_RGB(*img, result, RED);
    result->ChannelSize[GREEN] = compress_RGB(*img, result, GREEN);
    result->ChannelSize[BLUE] = compress_RGB(*img, result, BLUE);

    //printf_compressed_img(*result);

    return 1;
}


GLshort * reduce_raw_compressed_hue(GLshort* raw_compressed, unsigned long * size){
    
    GLshort * result = malloc( *size * sizeof(GLshort));
    assert(result);
    result[0] = 0;

    unsigned long empty_pt = 0, index_pt = 0;
    //printf("start compressing\n");
    for (unsigned long i = 0; i < *size; i+=2)//itere sur les indices de repetition de raw_compressed
    {   
        //printf(">>%hi\n ",raw_compressed[i]);
        if(raw_compressed[i] > maximum_repeat){ // cas simple, haute repetition, ecriture simple dans result
            index_pt = empty_pt++; // index_pt recoit un emplace vide
            result[index_pt] = raw_compressed[i];//savegarde du compteur
            result[empty_pt++] = raw_compressed[i+1];//sauvegarde de la valeur 
        }
        else if(result[index_pt] < 0 && (result[index_pt] - raw_compressed[i] > -32767)){// si compteur de redution encours
            //printf("here \n");
            //printf("%hi %hi %hi \n",result[0], result[1] , result[2]);
            //printf("%hi %hi %hi \n",result[index_pt] , raw_compressed[i], result[index_pt] - raw_compressed[i]);
                //printf("in if\n");
            result[index_pt] -= raw_compressed[i];
            for (unsigned long j = 0; j < raw_compressed[i]; j++){//ajoute le nombre de valeur necessaire
                result[empty_pt++] = raw_compressed[i+1];
            }
            
        }else{// nouvelle valeur a reduire
            //printf("getting new\n");
            index_pt = empty_pt++; // index_pt recoit un emplace vide
            result[index_pt] = raw_compressed[i] * -1; //flip du compteur
            for (unsigned long j = 0; j < raw_compressed[i]; j++){//ajoute le nombre de valeur necessaire
                result[empty_pt++] = raw_compressed[i+1];
            }
        }

    }
    
    // affiche le contenue apres reduction
    //printf("After compression : empty_pt = %ld\n[", empty_pt);
    //printf("After compression : index_pt = %ld\n[ ", index_pt);
    /*    for (unsigned long i = 0; i < empty_pt; i++)
    {
        printf("%hi ", result[i]);
    }
    printf(" ]\n ****************************************\n");
    */
    //free(raw_compressed); // libere la memoire de la compression brute
    //reduction de l'espace memoire
    //result = (GLbyte*) realloc(result, empty_pt * sizeof(GLbyte));

    *size = empty_pt; 
    return result;

} 

unsigned long compress_GLshort(GLshort * data, GLshort * storage, unsigned long size){
    GLshort counter =0;
    GLushort buffer = data[0];
    unsigned long k = 0;
    for (unsigned long i = 0; i < size; i++)
        {
            //printf("%hhu, ", data[i]);
            if(buffer == data[i]){//si suite de valeur identique
                if(counter < 32766){
                    counter++;
                }else{
                    //printf("<limit counter, push up new> ");
                    storage[k++] = counter; 
                    storage[k++] = buffer;
                    counter = 1;
                }
            //    printf(">i=%ld : same\n", i);
            }else{ // sinon 
            //    printf(">i=%ld : not same\n", i);
                //sauvegarde dans storage
                storage[k++] = counter; 
                storage[k++] = buffer;
                //reaffectation du nouveau buffer de test
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
    /*for (size_t i = 0; i < k; i++){
        printf("%hi ",tmp_storage[i]);
    }*/
    tmp_storage = reduce_raw_compressed_hue(tmp_storage,&k);
    dst->Hdata = tmp_storage;
    dst->ChannelSize[H] = k;
}

/**
 * @brief Create a compressed image in HSV format from HSV image
 * 
 * @param img 
 * @param result 
 */
void create_compressed_image_from_HSV(Image_HSV *img , Image_HSV_compressed *result){
    result->sizeX = img->sizeX;
    result->sizeY = img->sizeY;
    result->ChannelSize = malloc(3 * sizeof(unsigned long)); // tableau de 3 size h+s+v
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
    /*
    printf("======starting\n");
    for (size_t i = 0; i < size_src; i+=2)
    {
        printf("%hhi %hhu ", src[i], src[i+1]);
    }
    printf("\n");
    */
    GLbyte iter_buffer;
    GLubyte value_buffer;
    unsigned long size_counter = 0;
    while (j < size_src){
        iter_buffer = src[j];
        //printf("iter_buffer = %hhi\n", iter_buffer);
        if(iter_buffer > 0){//cas repetition simple
            //printf("positive case\n");
            value_buffer = src[j+1];
            for (size_t k = 0; k < iter_buffer; k++){
                //printf("value_buffer = %hhu ,pos = %ld\n",value_buffer,size_counter * coeff + pos );
                dst[size_counter++ * coeff + pos] = value_buffer;
            }
            j+=2;

        }else{ //cas repetition negative
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
    //printf("[%hhu %hhu]\n",img->data[0][0],img->data[0][1]);

    result->data = malloc( result->sizeX * result->sizeY * 3 * sizeof(GLubyte));
    assert(result->data);

    for (int i = 0; i < 3; i++){// iteration sur les 3 champs rgb
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

    unsigned long c;
    // channelSize // pixel data
    fprintf(fp, "%lu %lu %lu\n", img->ChannelSize[RED], img->ChannelSize[GREEN], img->ChannelSize[BLUE]);
    
    c = fwrite(img->data[RED], (size_t) 1, (size_t) img->ChannelSize[RED], fp);
    printf("wrote c : %ld  | expected : %ld\n", c, img->ChannelSize[RED]);
    c = fwrite(img->data[GREEN], (size_t) 1, (size_t) img->ChannelSize[GREEN], fp);
    printf("wrote c : %ld  | expected : %ld\n", c, img->ChannelSize[GREEN]);
    c = fwrite(img->data[BLUE], (size_t) 1, (size_t) img->ChannelSize[BLUE], fp);
    printf("wrote c : %ld  | expected : %ld\n", c, img->ChannelSize[BLUE]);

    //printf_compressed_img(*img);
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
    
    c = fwrite(img->SVdata[S], (size_t) 1, (size_t) img->ChannelSize[S], fp);
    printf("wrote c : %ld  | expected : %ld\n", c, img->ChannelSize[S]);
    c = fwrite(img->SVdata[V], (size_t) 1, (size_t) img->ChannelSize[V], fp);
    printf("wrote c : %ld  | expected : %ld\n", c, img->ChannelSize[V]);
    c = fwrite(img->Hdata, (size_t) sizeof(GLshort), (size_t) img->ChannelSize[H], fp);
    printf("wrote c : %ld  | expected : %ld\n", c, img->ChannelSize[H]);

    //printf_compressed_img(*img);
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
	printf("Size image %lu %lu => %d\n", img->sizeX, img->sizeY, size);
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
        printf("RED size: %lu\n", img_comp.ChannelSize[RED]);
        printf("GREEN size: %lu\n", img_comp.ChannelSize[GREEN]);
        printf("BLUE size: %lu\n", img_comp.ChannelSize[BLUE]);
        
        
        img_comp.data[RED] = malloc(img_comp.ChannelSize[RED] * sizeof(GLubyte));
        img_comp.data[GREEN] = malloc(img_comp.ChannelSize[GREEN] * sizeof(GLubyte));
        img_comp.data[BLUE] = malloc(img_comp.ChannelSize[BLUE] * sizeof(GLubyte));
        fread(img_comp.data[RED], 1, img_comp.ChannelSize[RED], fp);
        fread(img_comp.data[GREEN], 1, img_comp.ChannelSize[GREEN], fp);
        fread(img_comp.data[BLUE], 1, img_comp.ChannelSize[BLUE], fp);
        

        /*
        printf(">%hhi ",img_comp.data[RED][0] );
        for (size_t i = 1; i < 100; i++)
        {
            printf("%hhu ",img_comp.data[RED][i] );
        }
        printf("\n>%hhi ",img_comp.data[GREEN][0] );
        for (size_t i = 1; i < 100; i++)
        {
            printf("%hhu ",img_comp.data[GREEN][i] );
        }
        printf("\n>%hhi ",img_comp.data[BLUE][0] );
        for (size_t i = 1; i < 100; i++)
        {
            printf("%hhu ",img_comp.data[BLUE][i] );
        }
        */

        decompress_RGB(&img_comp, img);

    }else if(buff[1] == '8'){
        Image_HSV_compressed img_comp;
        img_comp.sizeX = img->sizeX;
        img_comp.sizeY = img->sizeY;
        img_comp.ChannelSize = malloc(3 * sizeof(unsigned long));
        img_comp.SVdata = malloc(2 * sizeof(GLubyte*));

        //printf("size: %lu %lu\n", img_comp.sizeX, img_comp.sizeY);
        if (fscanf(fp, "%lu %lu %lu\n", &img_comp.ChannelSize[RED], &img_comp.ChannelSize[GREEN], &img_comp.ChannelSize[BLUE]) != 3){
            fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
            exit(1);
        }
        printf("H size: %lu\n", img_comp.ChannelSize[H]);
        printf("S size: %lu\n", img_comp.ChannelSize[S]);
        printf("V size: %lu\n", img_comp.ChannelSize[V]);
        
        
        img_comp.SVdata[S] = malloc(img_comp.ChannelSize[S] * sizeof(GLubyte));
        img_comp.SVdata[V] = malloc(img_comp.ChannelSize[V] * sizeof(GLubyte));
        img_comp.Hdata = malloc(img_comp.ChannelSize[H] * sizeof(GLshort));
        fread(img_comp.SVdata[S], 1, img_comp.ChannelSize[S], fp);
        fread(img_comp.SVdata[V], 1, img_comp.ChannelSize[V], fp);
        fread(img_comp.Hdata, 1, img_comp.ChannelSize[H], fp);
        printf("reading done\n");
        
        Image_HSV img_hsv;
        decompress_HSV(&img_comp, &img_hsv);
        conv_HSV_img_to_RGB_img(&img_hsv, img);
        printf("End\n");
        
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
    double X = C * (1 - abs( (h/60) % 2 -1 ));
    //printf("X = %f\n", X);
    //printf("S = %f  V= %f  C= %f M = %f\n",_S, _V, C, M);

    //printf("R = %lf, G = %lf, B = %lf\n", R, G, B);
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

    *r = (_R + M) * 255 + 0.5;
    *g = (_G + M) * 255 + 0.5;
    *b = (_B + M) * 255 + 0.5;
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
 * @param dst ptr to destination RGB image
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
    
    for (size_t i = 0; i < 2; i++)
    {
        decompress_GLubytes(img->SVdata[i], result->SVdata[i], img->ChannelSize[i], i, 2);
    }
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
void decompress_GLshort(GLshort * src, GLshort * dst, unsigned long size_src){
    unsigned long j = 0;
    GLshort iter_buffer;
    GLshort value_buffer;
    unsigned long ptr_empty = 0;
    while (j < size_src){
        iter_buffer = src[j];
        //printf("iter_buffer = %hi\n", iter_buffer);
        if(iter_buffer > 0){//cas repetition simple
            //printf("positive case\n");
            value_buffer = src[j+1];
            for (size_t k = 0; k < iter_buffer; k++){
                //printf("value_buffer = %hhu ,pos = %ld\n",value_buffer,ptr_empty);
                dst[ptr_empty++ ] = value_buffer;
            }
            j+=2;

        }else{ //cas repetition negative
            //printf("negative case\n");
            iter_buffer = iter_buffer * -1;

            for (size_t k = 0; k < iter_buffer; k++){
                value_buffer = src[++j];
                //printf("value_buffer = %hhu ,pos = %ld\n",value_buffer,ptr_empty  );
                dst[ptr_empty++ ] = value_buffer;
            }
            j++;
                        
        }

    }
    
}



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