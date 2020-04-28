#include "compress.h"


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


        img_comp.Hdata = malloc(img_comp.ChannelSize[H] * sizeof(GLshort));
        if((r = fread(img_comp.Hdata, (size_t) sizeof(GLshort),(size_t) img_comp.ChannelSize[H], fp)) != img_comp.ChannelSize[H] ){
            fprintf(stderr,"Failed to read data");
            exit(1);
        }

        printf("H blocs read = %lu | expected = %lu\n", r, img_comp.ChannelSize[H]);

        Image_HSV img_hsv;
        decompress_HSV(&img_comp, &img_hsv);
        conv_HSV_img_to_RGB_img(&img_hsv, img);
    }
    fclose(fp);
    return 1;
   

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