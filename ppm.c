#include "ima.h"

#define INSPIREPAR "RPFELGUEIRAS"
#define CREATOR "JJ"
#define RGB_COMPONENT_COLOR 255

int ImageLoad_PPM(char *filename, Image *img)
{
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
    if (buff[0] != 'P' || buff[1] != '6') {
         fprintf(stderr, "Invalid image format (must be 'P6')\n");
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
    if (rgb_comp_color!= RGB_COMPONENT_COLOR) {
         fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
         exit(1);
    }
	/* allocation memoire */
	size = img->sizeX * img->sizeY * 3;
	printf("Size image %lu %lu => %d\n", img->sizeX, img->sizeY, size);
	img->data = (GLubyte *) malloc ((size_t) size * sizeof (GLubyte));
	assert(img->data);
									

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
	for (b = 0; b < img->sizeY / 2; b++) {
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
    fclose(fp);
    return 1;
}
void imagesave_PPM(char *filename, Image *img)
{
    FILE *fp;
    //open file for output
    fp = fopen(filename, "wb");
    if (!fp) {
         fprintf(stderr, "Unable to open file '%s'\n", filename);
         exit(1);
    }

    //write the header file
    //image format
    fprintf(fp, "P6\n");

    //comments
    fprintf(fp, "# Created by %s\n",CREATOR);

    //image size
    fprintf(fp, "%lu %lu\n",img->sizeX,img->sizeY);

    // rgb component depth
    fprintf(fp, "%d\n",RGB_COMPONENT_COLOR);

    // pixel data
    fwrite(img->data, (size_t) 1, (size_t) (3 * img->sizeX * img->sizeY), fp);
    fclose(fp);
}

