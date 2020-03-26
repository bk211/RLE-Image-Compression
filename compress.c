#include "compress.h"
// maximum de repetition autorise pour une reduction SGI
const unsigned long maximum_repeat = 2;



void print_image(Image img){
    for (size_t i = 0; i < img.sizeY; i++)
    {
        for (size_t j = 0; j < img.sizeX; j++)
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
            printf("%hhu, ", data[i]);
            if(buffer == data[i]){//si suite de valeur identique
                if(counter < 127){
                    counter++;
                }else{
                    printf("<limit counter, push up new> ");
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
    unsigned long k = compress_GLubyte(img.data, tmp_storage, color, img.sizeX * img.sizeY, T_RGB);
    
    // affiche le contenue de tmp_storage
    /*
    printf("\n>>>k = %ld\n[ ", k);
    for (size_t i = 0; i < k; i++)
    {
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

    unsigned long empty_pt = 0, index_pt = 0;
    //printf("start compressing\n");
    for (unsigned long i = 0; i < *size; i+=2)//itere sur les indices de repetition de raw_compressed
    {   
        //printf(">>%hhi\n ",raw_compressed[i]);
        if(raw_compressed[i] > maximum_repeat){ // cas simple, haute repetition, ecriture simple dans result
            index_pt = empty_pt++; // index_pt recoit un emplace vide
            result[index_pt] = raw_compressed[i];//savegarde du compteur
            result[empty_pt++] = raw_compressed[i+1];//sauvegarde de la valeur 
        }
        else if(result[index_pt] < 0 && (result[index_pt] - raw_compressed[i] > -128)){// si compteur de redution encours
            //printf("here \n");
            //printf("%hhi %hhi %hhi \n",result[0], result[1] , result[2]);
            //printf("%hhi %hhi %hhi \n",result[index_pt] , raw_compressed[i], result[index_pt] - raw_compressed[i]);
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
    printf("After compression : empty_pt = %ld\n[", empty_pt);
    //printf("After compression : index_pt = %ld\n[ ", index_pt);
    for (unsigned long i = 0; i < empty_pt; i++)
    {
        printf("%hhi ", result[i]);
    }
    printf(" ]\n ****************************************\n");

    free(raw_compressed); // libere la memoire de la compression brute
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




void save_compressed_image(char * filename, Image_RGB_compressed * img){
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
    //fprintf(fp, "%d\n",RGB_COMPONENT_COLOR);

    // pixel data
    fwrite(img->data[RED], (size_t) 1, (size_t) img->sizeChannel[RED], fp);
    fwrite(img->data[GREEN], (size_t) 1, (size_t) img->sizeChannel[GREEN], fp);
    fwrite(img->data[BLUE], (size_t) 1, (size_t) img->sizeChannel[BLUE], fp);
    fclose(fp);
}


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

void conv_RGB_HSV(Image src, Image_HSV dst){
    short h;
    GLubyte s,v;
    unsigned long size = src.sizeX * src.sizeY;
    for (size_t i = 0; i < size; i++){
        rgb_to_hsv(src.data[i*3], src.data[i*3+1], src.data[i*3+2], &h, &s, &v);
        dst.Hdata[i] = h;
        dst.SVdata[S][i] = s;
        dst.SVdata[V][i] = v;
    }
    
}


Image_HSV conv_RGB_img_to_HSV_img(Image src){
    Image_HSV result;
    result.sizeX = src.sizeX;
    result.sizeY = src.sizeY;
    result.Hdata = malloc( result.sizeX * result.sizeY * sizeof(result.Hdata));
    result.SVdata = malloc( 2 * sizeof(result.SVdata));    
    result.SVdata[S] = malloc( result.sizeX * result.sizeY * sizeof(result.SVdata[S]));
    result.SVdata[V] = malloc( result.sizeX * result.sizeY * sizeof(result.SVdata[V]));
    conv_RGB_HSV(src, result);
    return result;
}


Image_RGB_compressed create_compressed_image_from_RGB(Image img){
    Image_RGB_compressed result;
    result.sizeX = img.sizeX;
    result.sizeY = img.sizeY;
    result.data = malloc( 3 * sizeof( result.data) ); // 3 ptr vers 3 tableau de donner non alloue
    assert(result.data);
    result.sizeChannel = malloc( 3 * sizeof( result.sizeChannel));// tableau de 3 size rgb
    assert(result.sizeChannel);
    result.sizeChannel[RED] = compress_RGB(img, &result, RED);
    result.sizeChannel[GREEN] = compress_RGB(img, &result, GREEN);
    result.sizeChannel[BLUE] = compress_RGB(img, &result, BLUE);
    return result;
}


GLshort * reduce_raw_compressed_hue(GLshort* raw_compressed, unsigned long * size){
    
    GLshort * result = malloc( *size * sizeof(GLshort));
    assert(result);

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
        else if(result[index_pt] < 0 && (result[index_pt] - raw_compressed[i] > -128)){// si compteur de redution encours
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
    printf("After compression : empty_pt = %ld\n[", empty_pt);
    //printf("After compression : index_pt = %ld\n[ ", index_pt);
    for (unsigned long i = 0; i < empty_pt; i++)
    {
        printf("%hi ", result[i]);
    }
    printf(" ]\n ****************************************\n");

    free(raw_compressed); // libere la memoire de la compression brute
    //reduction de l'espace memoire
    //result = (GLbyte*) realloc(result, empty_pt * sizeof(GLbyte));

    *size = empty_pt; 
    return result;

}

unsigned long compress_GLshort(GLshort * data, GLshort * storage, unsigned long size){
    unsigned long k = 0;
    GLushort buffer = data[0];
    GLshort counter =0;
    for (unsigned long i = 0; i < size; i++)
        {
            printf("%hhu, ", data[i]);
            if(buffer == data[i]){//si suite de valeur identique
                if(counter < 32767){
                    counter++;
                }else{
                    printf("<limit counter, push up new> ");
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

void compress_SV(Image_HSV img, Image_HSV_compressed *dst, int type){
    GLbyte * tmp_storage = malloc( img.sizeX * img.sizeY * 2 * sizeof(tmp_storage)); // compression brute, la pire cas possible est un dedoublement de memoire
    assert(tmp_storage);
    unsigned long k = compress_GLubyte(img.SVdata[type], tmp_storage, SV, img.sizeX * img.sizeY, T_HSV);

    tmp_storage = reduce_raw_compressed(tmp_storage, &k);
    dst->SVdata[type] = (GLubyte*)tmp_storage;
    dst->sizeChannel[type] = k;
}

void compress_H(Image_HSV img, Image_HSV_compressed *dst){
    GLshort * tmp_storage = malloc( img.sizeX * img.sizeY * 2 * sizeof(GLshort));
    assert(tmp_storage);
    unsigned long k = compress_GLshort(img.Hdata, tmp_storage, img.sizeX * img.sizeY);
    printf(">>\n");
    for (size_t i = 0; i < k; i++)
    {
        printf("%hi ",tmp_storage[i]);
    }
    tmp_storage = reduce_raw_compressed_hue(tmp_storage,&k);
    dst->Hdata = tmp_storage;
    dst->sizeChannel[2] = k;
}

Image_HSV_compressed create_compressed_image_from_HSV(Image_HSV img){
    Image_HSV_compressed result;
    result.sizeX = img.sizeX;
    result.sizeY = img.sizeY;
    result.sizeChannel = malloc(3 * sizeof(result.sizeChannel)); // tableau de 3 size h+s+v
    assert(result.sizeChannel);
    result.SVdata = malloc(2 * sizeof( result.SVdata));
    assert(result.SVdata);
    compress_H(img, &result);
    compress_SV(img, &result, S);
    compress_SV(img, &result, V);


    return result;

}


Image decompress_RGB(Image_RGB_compressed img){
    Image result;
    result.sizeX = img.sizeX;
    result.sizeY = img.sizeY;
    result.data = malloc( result.sizeX * result.sizeY * 3 * sizeof(GLubyte));

    size_t j;
    GLbyte iter_buffer;
    GLubyte value_buffer;
    size_t size_counter;
    for (size_t i = 0; i < 3; i++){// iteration sur les 3 champs rgb
        unsigned long channel_size = img.sizeChannel[i];//obtiens la taille compresse'
        j = 0;
        size_counter = 0;
        while (j < channel_size)
        {
            iter_buffer = img.data[i][j];
            //printf("iter_buffer = %hhi\n", iter_buffer);
            if(iter_buffer > 0){//cas repetition simple
                //printf("iter case\n");
                value_buffer = img.data[i][j+1];
                for (GLbyte k = 0; k < iter_buffer; k++)
                {
                    //printf("%hhu ", value_buffer);
                    //printf(">%ld, \n" ,size_counter *3 + i );
                    result.data[size_counter++ * 3 + i] = value_buffer;
                }
                j+=2;
            }else{ // cas repetition negative
                //printf("negative case\n");
                for (GLbyte k = 0; k > iter_buffer; k++){
                    value_buffer = img.data[i][++j];
                    result.data[size_counter++ * 3 + i] = value_buffer;
                }
            }
        }
    }

    return result;
}