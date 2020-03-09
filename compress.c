#include "compress.h"
// maximum de repetition autorise pour une reduction SGI
const unsigned long maximum_repeat = 2;



void print_image(Image img){
    for (size_t i = 0; i < img.sizeY; i++)
    {
        for (size_t j = 0; j < img.sizeX; j++)
        {
            printf("[%hhi,%hhi,%hhi]",img.data[i*img.sizeY*3 + j*3],img.data[i*img.sizeY*3 + j*3+1],img.data[i*img.sizeY*3 + j *3 +2]);
        }
        printf("\n");
    }
    
}



unsigned long compress_loop(Image img, Image_compressed* dst,unsigned long sizeX, unsigned long sizeY, int color){
    
    /// >>>> a tester sur repetition > 255, comportement traiter mais non tested

    GLubyte buffer = img.data[color];
    GLbyte counter =0;
    GLbyte * tmp_storage = malloc( sizeX*sizeY * 2 * sizeof(GLubyte)); // compression brute, la pire cas possible est un dedoublement de memoire
    assert(tmp_storage);
    unsigned long k = 0;

    for (unsigned long i = color; i < sizeX * sizeY * 3; i+=3)
    {
        printf("%hhi, ", img.data[i]);
        if(buffer == img.data[i]){//si suite de valeur identique
            if(counter < 127){
                counter++;
            }else{
                printf("<limit counter, push up new> ");
                tmp_storage[k++] = counter; 
                tmp_storage[k++] = buffer;
                counter = 1;
            }
        //    printf(">i=%ld : same\n", i);
        }else{ // sinon 
        //    printf(">i=%ld : not same\n", i);
            //sauvegarde dans tmp_storage
            tmp_storage[k++] = counter; 
            tmp_storage[k++] = buffer;
            //reaffectation du nouveau buffer de test
            counter = 1;
            buffer = img.data[i];
        }
    }

    tmp_storage[k++] = counter; 
    tmp_storage[k++] = buffer;
    

    // affiche le contenue de tmp_storage
    printf("\n>>>k = %ld\n[ ", k);
    for (size_t i = 0; i < k; i++)
    {
        printf("%hhi ",tmp_storage[i]);
    }
    printf(" ]\n  ========================= \n");

    //reduit le resultat brute via la methode SGI
    tmp_storage = reduce_raw_compressed(tmp_storage, &k);
    dst->data[color] = tmp_storage; // stockage dans l'image dst

    return k;
}

GLbyte * reduce_raw_compressed(GLbyte* raw_compressed, unsigned long *size){
    
    GLbyte * result = malloc( *size * sizeof(GLbyte));
    assert(result);

    unsigned long empty_pt = 0, index_pt = 0;
    
    for (unsigned long i = 0; i < *size; i+=2)//itere sur les conteurs de repetition
    {   
        //printf("%hi ",raw_compressed[i]);
        if(raw_compressed[i] > maximum_repeat){ // cas simple, haute repetition, ecriture simple dans result
            index_pt = empty_pt++; // index_pt recoit un emplace vide
            result[index_pt] = raw_compressed[i];//savegarde du compteur
            result[empty_pt++] = raw_compressed[i+1];//sauvegarde de la valeur 
        }
        else if(result[index_pt] < 0){// si compteur de redution encours
            result[index_pt] -= raw_compressed[i];
            for (unsigned long j = 0; j < raw_compressed[i]; j++){//ajoute le nombre de valeur necessaire
                result[empty_pt++] = raw_compressed[i+1];
            }
        }else{// nouvelle valeur a reduire
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
    result = (GLbyte*) realloc(result, empty_pt * sizeof(GLbyte));

    *size = empty_pt; 
    return result;

}

void write_compressed_image(Image_compressed img){

}



Image_compressed compress(Image img){
    unsigned long sizeX = img.sizeX;
    unsigned long sizeY = img.sizeY;
    Image_compressed result;
    result.sizeX = sizeX;
    result.sizeY = sizeY;
    result.data = malloc( 3 * sizeof( result.data) );    
    assert(result.data);
    result.sizeChannel = malloc( 3 * sizeof( result.sizeChannel));
    assert(result.sizeChannel);
    result.sizeChannel[RED] = compress_loop(img, &result, sizeX, sizeY, RED);
    //result.sizeChannel[GREEN] = compress_loop(img, &result, sizeX, sizeY, GREEN);
    //result.sizeChannel[BLUE] = compress_loop(img, &result, sizeX, sizeY, BLUE);

    return result;
}