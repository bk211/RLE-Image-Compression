#include "compress.h"

Image gen_test_image(int i){
    Image foo;

    if(i == 0){

        foo.sizeX = 5;
        foo.sizeY = 5;
        foo.data = malloc( foo.sizeX* foo.sizeY * 3 *sizeof(GLubyte));
        for (size_t i = 0; i < foo.sizeX* foo.sizeY; i++)
        {
            if(i >= 45){
                foo.data[i] = 80;
            }else{
                foo.data[i] = 50;
            }
        } 
        return foo;
    }
    else if(i == 1){
        foo.sizeX = 5;
        foo.sizeY = 1;
        foo.data = malloc( foo.sizeX* foo.sizeY * 3 *sizeof(GLubyte));
        foo.data[0] = 40;
        foo.data[1] = 50;
        foo.data[2] = 50;

        foo.data[3] = 40;
        foo.data[4] = 60;
        foo.data[5] = 200;
        
        foo.data[6] = 200;
        foo.data[7] = 70;
        foo.data[8] = 60;
        
        foo.data[9] = 40;
        foo.data[10] = 70;
        foo.data[11] = 60;
        
        foo.data[12] = 40;
        foo.data[13] = 90;
        foo.data[14] = 60;
        
        return foo;
    }else if(i == 2){
        foo.sizeX = 130;
        foo.sizeY = 2;
        foo.data = malloc( foo.sizeX* foo.sizeY * 3 *sizeof(GLubyte));
        for (size_t i = 0; i < 2*130*3; i++)
        {
            foo.data[i] = 42;
        }
        
        return foo;
    }else if(i == 3){
        foo.sizeX = 100;
        foo.sizeY = 1;
        foo.data = malloc( foo.sizeX* foo.sizeY * 3 *sizeof(GLubyte));
        for (size_t i = 0; i < foo.sizeX * foo.sizeY * 3; i++)
        {
            foo.data[i] = i;
        }
        
        return foo;
    }else if(i == 4){
        foo.sizeX = 128;
        foo.sizeY = 1;
        foo.data = malloc( foo.sizeX* foo.sizeY * 3 *sizeof(GLubyte));
        for (size_t i = 0; i < foo.sizeX * foo.sizeY * 3; i++)
        {
            foo.data[i] = i;
        }
        foo.data[128*3 - 3] = 42;
        
        return foo;
    }
    else if(i == 5){
        foo.sizeX = 1024;
        foo.sizeY = 768;
        foo.data = malloc( foo.sizeX* foo.sizeY * 3 *sizeof(GLubyte));
        for (size_t i = 0; i < foo.sizeX * foo.sizeY * 3 /2 ; i++)
        {
            foo.data[i] = 40;
        }
        for (size_t i = foo.sizeX * foo.sizeY * 3 / 2; i < foo.sizeX * foo.sizeY * 3; i++)
        {
            foo.data[i] = 200;
        }
        
        return foo;
    }else if(i == 6){
        foo.sizeX = 200;
        foo.sizeY = 100;
        foo.data = malloc( foo.sizeX* foo.sizeY * 3 *sizeof(GLubyte));
        for (size_t i = 0; i < foo.sizeX * foo.sizeY ; i++){
            if(i % foo.sizeX < 100){

                foo.data[i * 3 ] = 120;
                foo.data[i * 3 +1] = 200;
                foo.data[i * 3 +2] = 100;
            }else{
                foo.data[i * 3 ] = 255;
                foo.data[i * 3 +1] = 0;
                foo.data[i * 3 +2] = 100;
            }
        }

        return foo;
    }else if(i == 7){
        foo.sizeX = 200;
        foo.sizeY = 100;
        foo.data = malloc( foo.sizeX* foo.sizeY * 3 *sizeof(GLubyte));
        for (size_t i = 0; i < foo.sizeX * foo.sizeY ; i++){
            if(i % foo.sizeX < 100){

                foo.data[i * 3 ] = 120;
                foo.data[i * 3 +1] = 200;
                foo.data[i * 3 +2] = 100;
            }else{
                foo.data[i * 3 ] = 255;
                foo.data[i * 3 +1] = 0;
                foo.data[i * 3 +2] = 100;
            }
        }

        return foo;
    }
    else{
        foo.sizeX = 1;
        foo.sizeY = 1;
        foo.data = malloc( foo.sizeX* foo.sizeY * 3 *sizeof(GLubyte));
        foo.data[0] = 100;
        foo.data[1] = 150;
        foo.data[2] = 255;
        return foo;
    }

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
    fprintf(fp, "# Created by %s\n","CC");

    //image size
    fprintf(fp, "%lu %lu\n",img->sizeX,img->sizeY);

    // rgb component depth
    fprintf(fp, "%d\n",255);

    // pixel data
    fwrite(img->data, (size_t) 1, (size_t) (3 * img->sizeX * img->sizeY), fp);
    fclose(fp);
}



int main(int argc, char const *argv[])
{
    GLubyte r1 = 218, r2 = 127, g1= 187,g2= 158, b1 =159 ,b2 =158;
    GLshort h;
    GLubyte bf1, bf2, br, bg, bb;
    
    rgb_to_hsv(r1, g1, b1, &h, &bf1, &bf2);
    printf("HSV %hi %hhu %hhu\n", h, bf1, bf2);
    hsv_to_rgb(h, bf1, bf2, &br, &bg, &bb);
    printf("RGB %hhu %hhu %hhu\n", br, bg, bb);
    /*
    FILE * fp1,* fp2;
    fp1 = fopen("raw.txt", "r");
    fp2 = fopen("read.txt", "r");
    
    for (size_t i = 0; i < 293510; i++){
        fscanf(fp1,"%hhu %hhu %hhu\n",&r1, &g1,&b1);
        fscanf(fp2,"%hhu %hhu %hhu\n",&r2, &g2,&b2);
        printf("[%hhu|%hhu] [%hhu|%hhu] [%hhu|%hhu] [%hhi %hhi %hhi][%hhi]\n",r1, r2, g1, g2, b1,b2, r2-r1, g2-g1, b2-b1, r2-r1 + g2-g1 + b2-b1);
    }
    
    fclose(fp1);
    fclose(fp2);*/
    
    /*
    GLshort h =199;
    GLubyte s=25,v=29, r,g,b;
    hsv_to_rgb(h, s,v , &r, &g, &b);
    printf("\nR: %hhu G: %hhu B: %hhu \n", r,g,b);*/

    //56,69,75
    //Image * f = malloc(sizeof (Image));
    //FILE *fp2, *fp3, *fp4, *fp5;
    //open file for output
    
    /*
    Image test_image = gen_test_image(7);
    imagesave_PPM("normal.ppm",&test_image);
    print_image(test_image);
    */

    /*
    Image_RGB_compressed *t = malloc(sizeof(Image_RGB_compressed));
    
    create_compressed_image_from_RGB(&test_image, t);
    printf("SIZE: %ld %ld\n", t->sizeX, t->sizeY);
    printf("SIZE C: %ld %ld %ld\n", t->ChannelSize[RED],t->ChannelSize[GREEN],t->ChannelSize[BLUE]);
    for (size_t i = 0; i < t->ChannelSize[RED]; i++)
    {
        printf("%hhi ",t->data[RED][i]);
    }
    printf("\n");
    for (size_t i = 0; i < t->ChannelSize[GREEN]; i++)
    {
        printf("%hhi ",t->data[GREEN][i]);
    }
    printf("\n");
    for (size_t i = 0; i < t->ChannelSize[BLUE]; i++)
    {
        printf("%hhi ",t->data[BLUE][i]);
    }

    save_compressed_RGB_image("compressed.ppm", t);
    Image *t6 = malloc(sizeof(Image));
    Image_load("compressed.ppm", t6);
    print_image(*t6);
    */


    /* 
    Image *t4 = malloc(sizeof(Image));
    Image_load("morty.ppm",t4);
    Image_RGB_compressed *t5 = malloc(sizeof(Image_RGB_compressed));
    create_compressed_image_from_RGB(t4, t5);
    //print_image(*t4);
    save_compressed_RGB_image("m.ppm", t5);
    */

    //printf("============================\n");
    
    /*
    Image *t3 = malloc(sizeof(Image));
    Image_load("m.ppm", t3);
    printf("start image=============\n");
    print_image(*t3);
    */
    
    //imagesave_PPM("after_getfp.ppm",t3);

    /*
    Image *t2 = malloc(sizeof (Image));
    decompress_RGB(t, t2);
    imagesave_PPM("after_decomp.ppm",t2);
    */
    
 
 //   print_image(test_image);
//    imagesave_PPM("normal.ppm", &test_image);
    /*Image_HSV bar = conv_RGB_img_to_HSV_img(test_image);
    
    for (size_t i = 0; i < bar.sizeX * bar.sizeY; i++){
        printf("%hi %hhu %hhu \n", bar.Hdata[i],bar.SVdata[S][i], bar.SVdata[V][i]);
    }
    Image_HSV_compressed compbar = create_compressed_image_from_HSV(bar);
    */
    //short tmp_h;
    //GLubyte tmp_s;
    //GLubyte tmp_l;

    // test conversion rgb -> hsv
    /*
    rgb_to_hsv(test_image.data[0], test_image.data[1], test_image.data[2], &tmp_h, &tmp_s, &tmp_l); 
    // attendu: 312, 82.5, 94.1
    rgb_to_hsv(100, 5, 120, &tmp_h, &tmp_s, &tmp_l);
    // attendu: 290 95.8 47.1
    rgb_to_hsv(51, 153, 204, &tmp_h, &tmp_s, &tmp_l);
    // attendu: 200 75 80
    */

    //test compression et sauvegarde
    
    /*
    Image_RGB_compressed *foo = malloc(sizeof(Image_RGB_compressed));
    create_compressed_image_from_RGB(&test_image, foo);
    save_compressed_RGB_image("test3.ppm",foo);
    */
    /*Image *kk = malloc(sizeof (Image));
    decompress_RGB(foo, kk);
    print_image(*kk);
    */
   
    
    //imagesave_PPM("test5.ppm", kk);
    //printf(">>\n");
    //Image * tmpp = malloc(sizeof (Image));
    //Image_load("test5.ppm", tmpp);
    //printf("printing image\n");
    //print_image(*tmpp);
    
    // tests de limites et de conversion
    /*
    GLubyte a = 60;
    GLubyte b = 68;
    GLubyte c = 67;// [0] -> [127] -> [128/-128] []
    printf("%hhu %hhu %hhu\n", a , b, c);
    printf("%hhi %hhi %hhi\n", a, b, c);
    printf("\n");
    printf("%hhu %hhu %hhu\n", a *-1, b *-1, c * -1);
    printf("%hhi %hhi %hhi\n", a *-1, b *-1, c * -1);
    
    printf("\n");
    printf("%hhu %hhu %hhu\n", a + b ,a +c , c);
    printf("%hhi %hhi %hhi\n", a + b, a + c, c);
    
    printf("\n");
    printf("%hhu %hhu %hhu\n", a *-1 -b  , b *-1 -c, c * -1);
    printf("%hhi %hhi %hhi\n", a *-1 -b, b *-1 - c, c * -1);
    
    printf("%hhi %hhi %hhi %hhi %hhi\n", -1, 1 , -127, -1 -1, -1 -1 < -127 );
    */


    return 0;  
}
