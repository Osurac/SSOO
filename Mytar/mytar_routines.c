#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

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
	// Complete the function

	int nCopied = 0;
	int c;
	//Comprobamos que lo recibido no es null
	if(origin == NULL || destination == NULL) return -1;

	do {
        	c = getc(origin); 
		if(c != EOF) { 
			putc(c, destination);
			nCopied++;
		}

	} while((c != EOF) && (nCopied != nBytes));

	return nCopied;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char*
loadstr(FILE * file)
{
	char *buffer = NULL;
	int i;
	int c;
	//Controlamos que file no sea null
	if (file == NULL) return NULL;

	// Reservamos memoria en base al tamaño maximo del path, inicializamos i como la variable de recorrido
	buffer = malloc(PATH_MAX);
	i = 0;

	// Añadimos al buffer la lectura de caracteres del archivo
	do {
		c = getc(file);
		if(c != '\n')
		buffer[i++] = c;

	} while(c != '\n');

	// Se añade '\0' al final del string
	buffer[i] = '\0';

	return buffer;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry*
readHeader(FILE * tarFile, int *nFiles)
{
	stHeaderEntry *arr = NULL;

	int n_files = 0;
	int i;

	//Controlamos que el tar no sea null
	if (tarFile == NULL) return NULL;

	// Leemos el numero de ficheros del tar y lo guardamos en n_files
	fread(&n_files, sizeof(int), 1, tarFile);
	fscanf(tarFile, "\n");

	// Reservamos memoria para el array
	arr = (stHeaderEntry*) malloc(sizeof(stHeaderEntry) * n_files);

	// Leemos la información del tar al array
	for(i = 0; i < n_files; i++){

		arr[i].name = malloc(PATH_MAX);
		arr[i].name = loadstr(tarFile);
		fread(&arr[i].size, sizeof(int), 1, tarFile);
		fscanf(tarFile, "\n");

	}

	(*nFiles) = n_files;

	return arr;
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
	FILE *iFile = NULL;
	FILE *tFile = NULL;
	stHeaderEntry *arr = NULL;

	int nBytes = 0;
	int i;

	if ((tFile = fopen(tarName, "wb")) == NULL) return EXIT_FAILURE;

	// Reservamos memoria para el array en base al numero de archivos
	arr = malloc(sizeof(stHeaderEntry) * nFiles);

	// nBytes es el tamaño de la cabecera del tar junto al byte de EOF 
	nBytes = (nFiles + 1) + sizeof(int) + nFiles * sizeof(unsigned int);

	// Copiamos la cabecera en el array
	for(i = 0; i < nFiles; i++){

		arr[i].name = malloc(strlen(fileNames[i]) + 1);
		strcpy(arr[i].name, fileNames[i]);
		nBytes += strlen(fileNames[i]) + 1;

	}

	// Nos movemos los bytes que ocupa la cabecera
	fseek(tFile, nBytes, SEEK_SET);

	// Copiamos el archivo en el mtar
	for(i = 0; i < nFiles; i++){

		iFile = fopen(fileNames[i], "rb");
		if (iFile == NULL) return EXIT_FAILURE;
		nBytes = copynFile(iFile, tFile, INT_MAX);
		fclose(iFile);
		arr[i].size = nBytes;

	}

	// Nos volvemos con el puntero al byte 0
	rewind(tFile);

	// Escribimos el numero de archivos comprimidos en el mtar
	fwrite(&nFiles, sizeof(int), 1, tFile);
	fwrite("\n", sizeof(char), 1, tFile);

	// Escribimos el nombre y el tamaño de cada archivo y liberamos el espacio.
	
	for(i = 0; i < nFiles; i++){

		fwrite(arr[i].name, strlen(fileNames[i]), 1, tFile);
		fwrite("\n", sizeof(char), 1, tFile);
		fwrite(&arr[i].size, sizeof(unsigned int), 1, tFile);
		fwrite("\n", sizeof(char), 1, tFile);
		free(arr[i].name);

	}

	printf("Fichero Mytar creado con exito\n");
	free(arr);
	fclose(tFile);

	return EXIT_SUCCESS;
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
	FILE *oFile = NULL;
	FILE *tFile = NULL;
	stHeaderEntry *arr = NULL;

	int nCopied;
	int n_files = 0;
	int i;

	if((tFile = fopen(tarName, "rb")) == NULL) return EXIT_FAILURE;

	//Leemos la cabecera del tar y la añadimos al array
	arr = readHeader(tFile, &n_files);

	// Creamos los ficheros 
	for(i = 0; i < n_files; i++){

		oFile = fopen(arr[i].name, "wb");
		nCopied = copynFile(tFile, oFile, arr[i].size);
		printf("[%d]: Creando fichero %s, tamano %d Bytes...Ok\n", i, arr[i].name, nCopied);
		fclose(oFile);

	}

	free(arr);
	fclose(tFile);

	return EXIT_SUCCESS;
}

/*
Given the name of a mtar file, shows all the contents (names + size) packed in it.
Returns 0 if everything went ok.
*/

int listTar(char tarName[]){

	FILE *tFile = NULL;
	stHeaderEntry *arr = NULL;

	int n_files = 0;
	int i;

	if((tFile = fopen(tarName, "rb")) == NULL) return EXIT_FAILURE;

	// Readds the header of the tarfile
	arr = readHeader(tFile, &n_files);

	// Prints the data contained in the tarFile
	for(i = 0; i < n_files; i++){

		printf("[%d]: Fichero %s, tamano %d Bytes\n", i, arr[i].name, arr[i].size);

	}

	free(arr);
	fclose(tFile);

	return 0;

}

/*
Añade un archivo dado al tar.
*/
int addFileToTar(int nFiles, char *fileNames[], char tarName[], char newTarName[]){

	FILE *iFile = NULL;
	FILE *tFile = NULL;
	FILE *newTFile = NULL;
	stHeaderEntry *arr = NULL;
	stHeaderEntry *newArr = NULL;

	int n_files = 0;
	int new_n_files = 0;
	int i;
	int nBytes = 0;

	if((tFile = fopen(tarName, "rb")) == NULL) return EXIT_FAILURE;

	if ((newTFile = fopen(newTarName, "wb")) == NULL) return EXIT_FAILURE;

	fread(&n_files, sizeof(int), 1, tFile);
	rewind(tFile);

	arr = (stHeaderEntry*) malloc(sizeof(stHeaderEntry)*n_files);

	// Leemos la cabecera del tar y guardamos la información en el array
	arr = readHeader(tFile, &n_files);

	// Añadimos el nuevo n de archivos al antiguo
	new_n_files = nFiles + n_files;

	// Creamos un nuevo array para guardar toda la info del Tar
	newArr = (stHeaderEntry*) malloc(sizeof(stHeaderEntry) * (new_n_files));

	// Copiamos el contenido del array inicial al nuevo
	for(i = 0; i < n_files; i++){

		newArr[i].name = malloc(strlen(arr[i].name) + 1);
		strcpy(newArr[i].name, arr[i].name);
		newArr[i].size = arr[i].size;

	}	

	for(i = 0; i < nFiles; i++){

		newArr[i + n_files].name = malloc(strlen(fileNames[i]) + 1);
		strcpy(newArr[i + n_files].name, fileNames[i]);
	}

	for(i = 0; i < n_files; i++)
		nBytes += strlen(arr[i].name) + 1;

	for(i = 0; i < nFiles; i++)
		nBytes += strlen(fileNames[i]) + 1;

	nBytes += (nFiles + n_files + 1) + sizeof(int) + (new_n_files) * sizeof(unsigned int);

	//Nos movemos con el puntero al header
	fseek(newTFile, nBytes, SEEK_SET);

	// Copiamos el antiguo tar en el nuevo
	for(i = 0; i < n_files; i++)
		copynFile(tFile, newTFile, newArr[i].size);

	// Añadimos el nuevo archivo al tar
	for(i = 0; i < nFiles; i++){

		iFile = fopen(fileNames[i], "rb");
		if(iFile == NULL) return EXIT_FAILURE;
		nBytes = copynFile(iFile, newTFile, INT_MAX);
		newArr[i+n_files].size = nBytes;
		fclose(iFile);

	}

	//Nos volvemos al byte 0
	rewind(newTFile);

	// Escribimos el numero de archivos comprimidos
	fwrite(&new_n_files, sizeof(int), 1, newTFile);
	fwrite("\n", sizeof(char), 1, newTFile);

	// Escribimos la cabecera
	for(i = 0; i < new_n_files; i++){

		fwrite(newArr[i].name, strlen(newArr[i].name), 1, newTFile);
		fwrite("\n", sizeof(char), 1, newTFile);
		fwrite(&newArr[i].size, sizeof(unsigned int), 1, newTFile);
		fwrite("\n", sizeof(char), 1, newTFile);
		free(newArr[i].name);

	}

	for(i = 0; i < n_files; i++)
		free(arr[i].name);

	printf("Fichero %s, tamano %d Bytes, anadido con exito.\n", fileNames[0], newArr[new_n_files-1].size);

	free(arr);
	free(newArr);
	fclose(newTFile);
	fclose(tFile);

	return EXIT_SUCCESS;

}
/*Elimina un archivo del tar*/
int removeFileFromTar(int nFiles, char *fileNames[], char tarName[], char newTarName[]){

	FILE *tFile = NULL;
	FILE *newTFile = NULL;
	stHeaderEntry *arr = NULL;
	stHeaderEntry *newArr = NULL;

	int n_files = 0;
	int new_n_files = 0;
	int i, j;
	int nBytes = 0;
	int nBytesTar = 0;
	int posEliminar = 0;
	int eliminarTamano = 0;
	

	if((tFile = fopen(tarName, "rb")) == NULL) return EXIT_FAILURE;

	if ((newTFile = fopen(newTarName, "wb")) == NULL) return EXIT_FAILURE;

	fread(&n_files, sizeof(int), 1, tFile);
	rewind(tFile);

	arr = (stHeaderEntry*) malloc(sizeof(stHeaderEntry)*n_files);

	// Leemos la cabecera del tar y guardamos la información en el array
	arr = readHeader(tFile, &n_files);

	// Añadimos el nuevo n de archivos al antiguo
	new_n_files = n_files - nFiles;

	// Creamos un nuevo array para guardar toda la info del Tar
	newArr = (stHeaderEntry*) malloc(sizeof(stHeaderEntry)*new_n_files);

	// Copiamos el contenido del array inicial al nuevo
	
	for(i = 0; i < n_files; i++){
		if(strcmp(arr[i].name, fileNames[0])){
			newArr[j].name = malloc(strlen(arr[i].name) + 1);
			strcpy(newArr[j].name, arr[i].name);
			newArr[j].size = arr[i].size;
			j++;
		}
		else{	
			posEliminar = i;
			eliminarTamano = arr[i].size*sizeof(char);
		}
	}

	for(i = 0; i < new_n_files; i++)
		nBytes += strlen(newArr[i].name) + 1;

	nBytes += (new_n_files + 1) + sizeof(int) + (new_n_files) * sizeof(unsigned int);

	// Nos movemos los bytes que ocupa la cabecera
	fseek(newTFile, nBytes, SEEK_SET);

	//Nos movemos al byte 0 y cargamos la cabecera del tar con todos los archivos
	rewind(tFile);

	for(i = 0; i < n_files; i++)
		nBytesTar += strlen(arr[i].name) + 1;

	nBytesTar += (n_files + 1) + sizeof(int) + (n_files) * sizeof(unsigned int);

	//Nos movemos los bytes que ocupa la cabecera 
	fseek(tFile, nBytesTar SEEK_SET);

	for(i = 0; i < n_files; i++){
		if(posEliminar == i){
			fseek(tFile, fileSize, SEEK_CUR); 
		}else{
			copynFile(tFile, newTFile, arr[i].size);
		}
	}

	//Nos volvemos al byte 0
	rewind(newTFile);

	// Escribimos el numero de archivos comprimidos
	fwrite(&new_n_files, sizeof(int), 1, newTFile);
	fwrite("\n", sizeof(char), 1, newTFile);

	// Escribimos la cabecera
	for(i = 0; i < new_n_files; i++){

		fwrite(newArr[i].name, strlen(newArr[i].name), 1, newTFile);
		fwrite("\n", sizeof(char), 1, newTFile);
		fwrite(&newArr[i].size, sizeof(unsigned int), 1, newTFile);
		fwrite("\n", sizeof(char), 1, newTFile);
		free(newArr[i].name);

	}

	for(i = 0; i < n_files; i++)
		free(arr[i].name);

	printf("Fichero: %s, tamano: %d Bytes, eliminado con exito.\n", fileNames[0], fileSize);

	free(arr);
	free(newArr);
	fclose(newTFile);
	fclose(tFile);

	return EXIT_SUCCESS;


}
