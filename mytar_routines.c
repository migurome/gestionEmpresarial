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
	char buf;
	int n=0;
	
	buf = getc(origin);
	while ((n < nBytes) && (buf != EOF)){
		putc(buf, destination);
		n++;
		buf = getc(origin);
	}
	fseek(origin, -1, SEEK_CUR);
	
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
	int n, size=0;

	do{
		n = getc(file);
		size++;
	}while((n!=(int)'\0') && (n != EOF));

	if(n==EOF){
		return -1;
	}

	if((*buf = (char*)malloc(size))==NULL){
		return -1;
	}
	
	fseek(file, -size, SEEK_CUR);

	fread(*buf, 1, size, file);

	return 0;
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
	
	*nFiles = leerNum(tarFile);
	
	p = (stHeaderEntry*)malloc((*nFiles)*sizeof(stHeaderEntry));
	*header = p;
	
	for(i = 0; i<*nFiles;++i){
		loadstr(tarFile, &p[i].name);
		p[i].size = leerNum(tarFile);
	}

	return 0;
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
int
createTar(int nFiles, char *fileNames[], char tarName[])
{
	FILE *tarFile = fopen(tarName,"w");
	char num[100];
	stHeaderEntry *header = (stHeaderEntry*)malloc(nFiles * sizeof(stHeaderEntry));		// Reservamos espacio para las cabeceras
	int i, tam;
	char caracter;

	for(i = 0; i < nFiles; i++){								//Rellenamos las cabeceras
		header[i].name = (char*)malloc(strlen(fileNames[i])); //+1 del barra 0 LO HE QUITADO PORQUE LUEGO HAY QUE AÑADIR EL \0 AL FINAL
		strcpy(header[i].name, fileNames[i]);					//Copiamos el valor de filenames a header[i]
	}

	//AVANZAR CURSOR HASTA DEJAR ESPACIO PARA EL HEADER
	//sprintf(num, "%d", nFiles);
	fseek(tarFile, sizeof(int)+1, SEEK_SET); //Dejamos espacio para nFiles
	for(i = 0; i < nFiles; i++){ //Dejamos espacio para el nombre y el entero del tamaño
		//sprintf(num, "%d", header[i].size); 
		/*
			Al reservar espacio para header.size al principio (antes de leer el archivo) el tamaño es 0, una cifra, y al pasarlo a cadena 
			queda una cadena de un caracter porque el tamaño es 0, pero luego al leerlo y poner un tamaño de fichero mayor de 1 cifra de bytes
			y escribimos posteriomente el header escribe esas X cifras en el espacio que solo se reservo para una
			
			Solucion: escribir enteros en vez de cadenas
		*/
		fseek(tarFile, strlen(header[i].name)+1, SEEK_CUR);
		fseek(tarFile, sizeof(int)+1, SEEK_CUR); //+2 POR LOS DOS '\0' TANTO DEL NOMBRE COMO DEL TAMAÑO
	}
	

	//EMPEZAMOS A ESCRIBIR LOS DATOS DE LOS FICHEROS
	
	for(i = 0; i < nFiles; i++){ 		
			
		FILE *fichero = fopen(fileNames[i], "r"); 	
		caracter = fgetc(fichero);							
		tam = 0;
		if(caracter != EOF){								
			while(caracter != EOF){
				fputc(caracter, tarFile);
				caracter = fgetc(fichero);					
				tam++;
			}
			header[i].size = tam;	
		}
		fclose(fichero);
	}
	//VOLVEMOS A PONER EL CURSOR AL COMIENZO
	rewind(tarFile);

	//ESCRIBIMOS EL HEADER
	fwrite(&nFiles, sizeof(int),1,  tarFile);//PORQUE ESTO NO VA
	//sprintf(num, "%d", nFiles);//Escribimos el entero del tamaño
	//fwrite(num,1,strlen(num),tarFile);
	fputc('\0',tarFile);
	for(i = 0; i < nFiles; i++){ //Dejamos espacio para el nombre y el entero del tamaño
		fwrite(header[i].name, 1, strlen(header[i].name), tarFile);
		fputc('\0',tarFile);
		//sprintf(num, "%d", header[i].size);
		//fwrite(num,1,strlen(num),tarFile);
		fwrite(&header[i].size, sizeof(int),1,  tarFile);
		fputc('\0',tarFile);
	}
	
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
int
extractTar(char tarName[])
{
	FILE *tarFile = fopen(tarName,"r");
	int nFiles, i=0;
	stHeaderEntry *header;
	
	readHeader(tarFile, &header, &nFiles);
	
	for (i=0; i < nFiles; i++)
	{
		FILE * f = fopen(header[i].name, "w");
		copynFile(tarFile, f, header[i].size);
		fclose(f);
	}
	fclose(tarFile);
	return 0;
}

int leerNum (FILE * f)
{
	int *n;
	char c;
	/*c = fgetc(f);
	while(c!='\0')
	{
		n = n*10 + (int)c;
		c = fgetc(f);
	}*/
	fread(&n, sizeof(int), 1, f);
	fseek(f, 1, SEEK_CUR);
	printf("%d\n", n);
	return *n;
}
