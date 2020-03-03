#  Makefile 
#  Auteur : Fares BELHADJ
#  Email  : amsi@ai.univ-paris8.fr
#  Date   : 23/02/2006

#  Modificiation : Mehdi Aghayani
#  Email  : mehdi.aghayani@gmail.com
#  Commentaire : modifié afin de fonctionner avec OpenGL et MacOS X et libpng
#  
#  Université Paris VIII

SHELL = /bin/sh
#definition des commandes utilisees
CC = gcc
MACHINE= $(shell uname -s)
#declaration des options du compilateur
#PG_FLAGS = -DOPENGL_1_5

ifeq ($(MACHINE), Darwin)
GL_LDFLAGS = -framework OpenGL -framework GLUT -framework Cocoa
else
GL_LDFLAGS = -lGL -lglut -lGLU
endif

CFLAGS = -Wall
LDFLAGS = -lm $(GL_LDFLAGS)
#definition des fichiers et dossiers
PROGNAME = exec
HEADERS = bmp.h compress.h
SOURCES = main.c ppm.c compress.c
FILE = img/can_bottom2.ppm
OBJ = $(SOURCES:.c=.o)

all: $(PROGNAME)

$(PROGNAME): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(PROGNAME)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	@$(RM) $(PROGNAME) $(OBJ) *~ gmon.out core.* test

run:	$(PROGNAME) 
	./$(PROGNAME) $(FILE)

test:	compress_test.c compress.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< compress.o -o $@ 

rtest:	test
	./test