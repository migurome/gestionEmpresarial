#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"
#include <string.h>

/*Se declara como externa por si hay que imprimirla en algún momento, implica que esta declarada 
en algún otro archivo*/

extern char *use;

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int
copynFile(FILE * origin, FILE * destination, int nBytes)
{
	char buf, *buffer;
	int i, n = 0;
	
	buffer=(char *)malloc(nBytes+1); //Reservo espacio de memoria para el contenido del archivo a copiar
	
	n = (fread(buffer, 1, nBytes, origin) + fwrite(buffer, 1, nBytes, destination))/2; //Saco la media tanto de los bytes copiados como de los  
																					//escritos en el archivo, la cual deberia ser igual a
																					//nBytes si todo ha ido bien
	free(buffer); //Libero la memoria reservada
	
	return n;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * buf: parameter to return the read string. Buf is a
 * string passed by reference. 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly "built" in memory, return the starting 
 * address of the string (pointer returned by malloc()) in the buf parameter
 * (*buf)=<address>;
 * 
 * Returns: 0 if success, -1 if error
 */
int
loadstr(FILE * file, char **buf) //**buf es un paso por referencia
{
	int n, size = 0;
	
	do{					//Leo caracteres hasta encontrar un \0 que me delimite el tamaño de la cadena 
		n = getc(file);
		size++;
	}while((n!=(int)'\0') && (n != EOF));

	if(n==EOF){ //Si encuentro el final del archivo antes del final de la cadena entonces error
		return EXIT_FAILURE;
	}

	if((*buf = (char*)malloc(size))==NULL){ //Reservo espacio de memoria y en caso de fallo retorna error
		return EXIT_FAILURE;
	}
	
	fseek(file, -size, SEEK_CUR); //Pongo el cursor del fichero de nuevo al comienzo de la cadena

	fread(*buf, 1, size, file);  

	return EXIT_SUCCESS;
}


/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * header: output parameter. It is used to return the starting memory address
 * of an array that contains the (name,size) pairs read from the tar file
 * nFiles: output parameter. Used to return the number of files stored in
 * the tarball archive (first 4 bytes of the header)
 *
 * On success it returns EXIT_SUCCESS. Upon failure, EXIT_FAILURE is returned.
 * (both macros are defined in stdlib.h).
 */
int
readHeader(FILE * tarFile, stHeaderEntry ** header, int *nFiles)
{
	int i, n=0;
	char c;
	stHeaderEntry* p;

	*nFiles = leerNum(tarFile); //Leo el numero de archivos del mtar

	if((p = (stHeaderEntry*)malloc((*nFiles)*sizeof(stHeaderEntry)))==NULL)
		return EXIT_FAILURE;
	
	for(i = 0; i<*nFiles; i++){ //Leo el nombre de las cadenas y su tamaño y lo guardo en la estructura de la cabecera con todos los datos
		if(loadstr(tarFile, &p[i].name)==EXIT_FAILURE)	//de los archivos que contiene el tar
			return EXIT_FAILURE;		
		p[i].size = leerNum(tarFile);
	}

	*header = p; //El valor de header será el de p
	
	return EXIT_SUCCESS;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int createTar(int nFiles, char *fileNames[], char tarName[])
{
	if(!esTar(tarName)){ 
		printf("Incorrect file extension\n");
		return EXIT_FAILURE;
	}

	FILE *tarFile = fopen(tarName,"w"), *f;
	stHeaderEntry *header = (stHeaderEntry*)malloc(nFiles * sizeof(stHeaderEntry));		// Reservamos espacio para las cabeceras
	int i;
	char caracter, *buffer;

	//COMPROBACION DE LA EXISTENCIA DE TODOS LOS ARCHIVOS

	for(i = 0; i < nFiles; i++){		//si alguno no se puede abrir ya no se ejecuta mas programa						
		if((f=fopen(fileNames[i], "r"))==NULL){							
			printf("The file %s doesn't exist\n", fileNames[i]); 
			return EXIT_FAILURE;
		}
		else
			fclose(f);
	}	
	
	//CREACION DEL HEADER

	for(i = 0; i < nFiles; i++){								//Rellenamos las cabeceras
		if((header[i].name = (char*)malloc(strlen(fileNames[i]))) == NULL)
			return EXIT_FAILURE; 	
		strcpy(header[i].name, fileNames[i]);					//Copiamos el valor de filenames a header[i]
	}




	//AVANZAR CURSOR HASTA DEJAR ESPACIO PARA EL HEADER

	fseek(tarFile, sizeof(int)+1, SEEK_SET); //Dejamos espacio para nFiles
	for(i = 0; i < nFiles; i++){ //Dejamos espacio para el nombre y el entero del tamaño
		fseek(tarFile, strlen(header[i].name)+1, SEEK_CUR); //+1 para el \0 del nombre
		fseek(tarFile, sizeof(int)+1, SEEK_CUR); //+1 para el \0 del tamanio
	}
	

	//EMPEZAMOS A ESCRIBIR LOS DATOS DE LOS FICHEROS
	
	for(i = 0; i < nFiles; i++){ 		
			
		FILE *fichero = fopen(header[i].name, "r"); 	
		
		fseek(fichero, 0, SEEK_END);		//Desplazamos el cursor hasta el final del fichero para obtener el numero de bytes del mismo
		header[i].size = ftell(fichero);	//Lo guardamos en la estructura de la cabecera y volvemos a poner el cursor al principio
		fseek(fichero, 0, SEEK_SET);
		
		if((copynFile(fichero, tarFile, header[i].size)) != header[i].size){ //Copiamos el contenido del archivo al mtar y si el tamaño 
			fclose(fichero);													//que devuelve no es igual al que tenia que haber copiado
			printf("The %s file has been copied incorrectly\n", header[i].name);//entonces devuelve error
			return EXIT_FAILURE;	
		}

		fclose(fichero);
	}
	//VOLVEMOS A PONER EL CURSOR AL COMIENZO

	rewind(tarFile);

	//ESCRIBIMOS EL HEADER

	fwrite(&nFiles, sizeof(int),1, tarFile);
	fputc('\0',tarFile);
	for(i = 0; i < nFiles; i++){ 
		fwrite(header[i].name, 1, strlen(header[i].name), tarFile);
		fputc('\0',tarFile);
		fwrite(&header[i].size, sizeof(int),1,  tarFile);
		fputc('\0',tarFile);
	}
	
	free(header);
	fclose(tarFile);

	return 0;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int extractTar(char tarName[])
{
	if(!esTar(tarName)){ 
		printf("Incorrect file extension\n");
		return EXIT_FAILURE;
	}

	//COMPROBACION DE QUE EL ARCHIVO .mtar EXISTE

	FILE *tarFile, *f;
	if((tarFile = fopen(tarName,"r")) == NULL){ 
		printf("The file %s doesn't exist\n", tarName);	
		return EXIT_FAILURE;
	}

	int nFiles, i=0;
	stHeaderEntry *header;
	
	//OBTENCION DE LOS DATOS DE LA CABECERA

	readHeader(tarFile, &header, &nFiles); //Obtencion de la cabecera del archivo .mtar

	//CREACION Y COPIADO DEL CONTENIDO DEL ARCHIVO .mtar EN CADA UNO DE LOS ARCHIVOS CONTENIDOS

	for (i=0; i < nFiles; i++)
	{	
		f = fopen(header[i].name, "w");
		if((copynFile(tarFile, f, header[i].size)) != header[i].size){
			fclose(f);
			printf("The %s file has been copied incorrectly\n", header[i].name);	
			return EXIT_FAILURE;
		}

		fclose(f);
	}
	
	fclose(tarFile);
	return 0;
}

int listTar (char tarName[])
{
	if(!esTar(tarName)){ 
		printf("Incorrect file extension\n");
		return EXIT_FAILURE;
	}

	//COMPROBACION DE QUE EL ARCHIVO .mtar EXISTE

	FILE *tarFile, *f;
	if((tarFile = fopen(tarName,"r")) == NULL){ 
		printf("The file %s doesn't exist\n", tarName);	
		return EXIT_FAILURE;
	}

	int nFiles, i=0;
	stHeaderEntry *header;
	
	//OBTENCION DE LOS DATOS DE LA CABECERA

	readHeader(tarFile, &header, &nFiles); //Obtencion de la cabecera del archivo .mtar
	
	for (i=0; i < nFiles; i++)
		printf("Nombre: %s\t%d bytes\n", header[i].name, header[i].size); 
	
	fclose(tarFile);
	
	return EXIT_SUCCESS;
}

/** 
 * Funcion para leer un entero de un archivo
 */

int leerNum (FILE * f)
{
	int n;

	fread(&n, sizeof(int), 1, f);
	fseek(f, 1, SEEK_CUR);
	
	return n;
}

/** 
 * Funcion para la comprobacion de que el fichero tiene extension .mtar
 */

int esTar (char name[])
{
	int j = strlen(name)-1, ok = 1, tam = 0;

	while(j >= 0  && ok == 1) 
	{

		switch (tam)
		{
			case 0:
				if(name[j] != 'r')
					ok = 0;
				break;
			case 1:
				if(name[j] != 'a')
					ok = 0;
				break;
			case 2:
				if(name[j] != 't')
					ok = 0;
				break;
			case 3:
				if(name[j] != 'm')
					ok = 0;
				break; 
			case 4:
				if(name[j] != '.' || j == 0)
					ok = 0;
				break;
		}
		tam++;
		j--;
	}

	return ok;
}