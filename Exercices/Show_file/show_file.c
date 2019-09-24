

#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[]) {
	FILE* file=NULL;
	int c;
        char buffer[1]; 

	if (argc!=2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	/* Open file */
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	/* Read file byte by byte */

      //En fread mantenemos c como variable de control dado que devuelve un error al entrar en EOF, le pasamos por parametro el buffer donde guardarmos los char, el tamaño en bytes, el numero de bytes (1 a 1) y el origen, en este caso file
	
      while ((c = fread(buffer, sizeof(char), 1, file))){

      //Eliminamos la variable ret dado que no la necesitaremos como control dado que imprimimos por pantalla.

      //En fwrite pasamos por parametro el buffer, tamaño en bytes, numero de bytes, y el destino
	fwrite(buffer, sizeof(char), 1, stdout);

      }

	fclose(file);
	return 0;
}
