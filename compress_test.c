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
    }
    else{
        foo.sizeX = 1;
        foo.sizeY = 1;
        foo.data = malloc( foo.sizeX* foo.sizeY * 3 *sizeof(GLubyte));
        foo.data[0] = 240;
        foo.data[1] = 42;
        foo.data[2] = 42;
        return foo;
    }

}


int main(int argc, char const *argv[])
{
    /*
    Image test_image = gen_test_image(2);
    print_image(test_image);
    Image_compressed foo = compress(test_image);

    save_compressed_image("Patate.ppm",&foo);
    */
    GLubyte a = (GLubyte)256;
    GLubyte b = (GLubyte)257;
    GLubyte c = (GLubyte)258;// [0] -> [127] -> [128/-128] []
    printf("%hhu %hhu %hhu\n", a , b, c);
    printf("%hhi %hhi %hhi\n", a, b, c);

    printf("%hhu %hhu %hhu\n", a *-1, b *-1, c * -1);
    printf("%hhi %hhi %hhi\n", a *-1, b *-1, c * -1);
    
    return 0;  
}
