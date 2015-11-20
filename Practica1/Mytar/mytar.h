#ifndef _MYTAR_H
#define _MYTAR_H

#include <limits.h>

//Sirve para indicar que opción nos han pasado en la llamada.
//La lectura de las opciones está ya resuelta

typedef enum{
  NONE,
  ERROR,
  CREATE,
  EXTRACT, 
  LIST
} flags;


// Estructura que va en el fichero Mtar, ruta1, tamanio1(B) es de tipo stHeaderEntry 
typedef struct {
  char* name;
  unsigned int size;
} stHeaderEntry;

int createTar(int nFiles, char *fileNames[], char tarName[]);
int extractTar(char tarName[]);
int listTar (char tarName[]);

#endif /* _MYTAR_H */
