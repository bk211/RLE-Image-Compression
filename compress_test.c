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
        foo.data[5] = 50;
        
        foo.data[6] = 40;
        foo.data[7] = 70;
        foo.data[8] = 60;
        
        foo.data[9] = 40;
        foo.data[10] = 80;
        foo.data[11] = 60;
        
        foo.data[12] = 40;
        foo.data[13] = 90;
        foo.data[14] = 50;
        
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
    else{
        foo.sizeX = 1;
        foo.sizeY = 1;
        foo.data = malloc( foo.sizeX* foo.sizeY * 3 *sizeof(GLubyte));
        foo.data[0] = 240;
        foo.data[1] = 42;
        foo.data[2] = 201;
        return foo;
    }

}


int main(int argc, char const *argv[])
{
    
    Image test_image = gen_test_image(3);
    print_image(test_image);
    Image_HSV bar = conv_RGB_img_to_HSV_img(test_image);
    for (size_t i = 0; i < bar.sizeX * bar.sizeY; i++){
        printf("%hi %hhu %hhu \n", bar.Hdata[i],bar.SVdata[S][i], bar.SVdata[V][i]);
    }
    

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
    
    //Image_compressed foo = compress(test_image);
    //save_compressed_image("Patate.ppm",&foo);
    

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
