TARGET = mytar
CC = gcc
CFLAGS = -g -Wall 
OBJS = mytar.o mytar_routines.o
SOURCES = $(addsuffix .c, $(basename $(OBJS)))
HEADERS = mytar.h

all: compila.c
	gcc mytar.o rut_mitar.o -o Ejemplo

compila.c:
	gcc -c rut_mitar.c mytar.c

clean: 
	rm -f *.o
