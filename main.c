#include <unistd.h>
#include "compress.h"
#include <time.h>
#include <GL/glut.h>
#include <GL/glu.h>	

Image *image;
Image_RGB_compressed * image_RGB_comp;
Image_HSV * image_HSV;
Image_HSV_compressed * image_HSV_comp;
clock_t cp;
int tps_init, tps_crt, dtps;

#define ESCAPE 27

void Keyboard(unsigned char key, int x, int y)  {
  switch(key){
  case ESCAPE :
    exit(0);                   
    break;
  default:
    fprintf(stderr, "Unused key\n");
  }
}

void Mouse(int button, int state, int x, int y) {

  switch(button){
  case GLUT_LEFT_BUTTON:
    break;
  case GLUT_MIDDLE_BUTTON:
    break;
  case GLUT_RIGHT_BUTTON:
    break;    
  }
  glutPostRedisplay();
}

int Init(char *s) {

  image = (Image *) malloc(sizeof(Image));
  
  if (image == NULL) {
    fprintf(stderr, "Out of memory\n");
    return(-1);
  }

  tps_init = (int) clock();
  if (Image_load(s, image)==-1) 
	  return(-1);
  tps_crt = (int) clock();
  dtps = tps_crt - tps_init;
  printf("Time for Image_load = %d | %ld sec\n", dtps, dtps/ CLOCKS_PER_SEC);
  

  printf("tailles %d %d\n",(int) image->sizeX, (int) image->sizeY);

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glutReshapeWindow(image->sizeX, image->sizeY);
  
  return (0);
}
int ReInit() {

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glutReshapeWindow(image->sizeX, image->sizeY);
  
  return (0);
}

void Display(void) {
  
  //GLint w, h;

  glClear(GL_COLOR_BUFFER_BIT);

  //w = glutGet(GLUT_WINDOW_WIDTH);
  //h = glutGet(GLUT_WINDOW_HEIGHT);

  glDrawPixels(image->sizeX, image->sizeY, GL_RGB, GL_UNSIGNED_BYTE, 
	       image->data);

  glFlush();
}

void Reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble)h);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void menuFunc(int item) {
  char s[256];

  switch(item){
  case 0:
    free_images(image, image_RGB_comp, image_HSV, image_HSV_comp);
    printf("All memory free\nGoodbye\n");
    exit(0);
    break;
  case 1:
    tps_init = (int) clock();
    free_images(NULL, image_RGB_comp, NULL, NULL);
    image_RGB_comp = (Image_RGB_compressed *) malloc(sizeof(Image_RGB_compressed));
    printf("Compression en cours...\n");
    create_compressed_image_from_RGB(image,  image_RGB_comp);
    printf("Fin de la compression \n");
    tps_crt = (int) clock();
    dtps = tps_crt - tps_init;
    printf("Time for RGB compression = %d | %ld sec\n", dtps, dtps/ CLOCKS_PER_SEC);
      
    printf("Entrer le nom pour l'image dans cette taille\n");    
    scanf("%s", &s[0]);
    tps_init = (int) clock();
    save_compressed_RGB_image(s, image_RGB_comp);
    printf("save succes\n");
    tps_crt = (int) clock();
    dtps = tps_crt - tps_init;
    printf("Time for RGB compression save = %d | %ld sec\n", dtps, dtps/ CLOCKS_PER_SEC);
  

    break;
  
  case 2:
    tps_init = (int) clock();
    free_images(NULL, NULL, image_HSV, image_HSV_comp);
    printf("Compression en cours...\n");
    image_HSV = (Image_HSV*) malloc(sizeof(Image_HSV));
    conv_RGB_img_to_HSV_img(image, image_HSV);
    image_HSV_comp = (Image_HSV_compressed*) malloc(sizeof(Image_HSV_compressed));
    create_compressed_image_from_HSV(image_HSV, image_HSV_comp);
    printf("Fin de la compression \n");
    tps_crt = (int) clock();
    dtps = tps_crt - tps_init;
    printf("Time for HSV compression = %d | %ld sec\n", dtps, dtps/ CLOCKS_PER_SEC);
  

    printf("Entrer le nom pour l'image dans cette taille\n");    
    scanf("%s", &s[0]);
    tps_init = (int) clock();
    save_compressed_HSV_image(s, image_HSV_comp);
    printf("save succes\n");
    tps_crt = (int) clock();
    dtps = tps_crt - tps_init;
    printf("Time for HSV compression save = %d | %ld sec\n", dtps, dtps/ CLOCKS_PER_SEC);
  
    break;
  
  case 3:
    printf("Entrer le nom pour l'image dans cette taille\n");    
    scanf("%s", &s[0]);
    imagesave_PPM(s, image);
    break;
  case 4:
    printf("Taille de l image : %d %d\n", (int) image->sizeX, (int) image->sizeY);
    break;
  case 5:
    printf("You have clicked this button\n");
    break;
    
  default:
    break;
  }
}

int main(int argc, char **argv) {  

  if (argc<2) {
    fprintf(stderr, "Usage : palette nom_de_fichier\n");
    exit(0);
  }

  glutInit(&argc, argv); 
  glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
  glutInitWindowSize(640,480);  
  glutInitWindowPosition(100, 100);  
  glutCreateWindow("VPUP8");  

  Init(argv[1]);

  glutCreateMenu(menuFunc);
  glutAddMenuEntry("Quit", 0);
  glutAddMenuEntry("Compresser sous RGB", 1);
  glutAddMenuEntry("Compresser sous HSV", 2);
  glutAddMenuEntry("Sauver", 3);
  glutAddMenuEntry("Informations", 4);
  glutAddMenuEntry("click me", 5);
  
  glutAttachMenu(GLUT_LEFT_BUTTON);

  glutDisplayFunc(Display);  
  glutReshapeFunc(Reshape);
  glutKeyboardFunc(Keyboard);
  
  glutMouseFunc(Mouse);

  glutMainLoop();  

  return 1;
}
