
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <ctype.h>
//incluimos ctype para poder usar isdigit
int main(int argc, char* argv[]) {
	FILE* file=NULL;
	int c;
	size_t n;
        char * buffer; 

	if (argc < 3) {
	   if(argc!=2){
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	    }
                fprintf(stderr,"Usage: %s <n_bytes>\n",argv[0]);
		exit(1);
	}

	
	/* Open file */
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	/*Leer n bytes*/
	n = *argv[2];

	if (!isdigit(n) || (n<='0')){
		
		fprintf(stderr,"No has introducido un numero de bytes correcto: %s\n",argv[2]);
		exit(1);
		
	}
	

	/* Read file byte by byte */

      //En fread mantenemos c como variable de control dado que devuelve un error al entrar en EOF, le pasamos por parametro el buffer donde guardarmos los char, el tamaño en bytes, el numero de bytes (1 a 1) y el origen, en este caso file. Usariamos fread(buffer, sizeof(char), 1, file) y fwrite(buffer, sizeof(char), 1, stdout) para 1 solo byte 

      buffer = (char*) malloc(n+1);
	
      while ((c = fread(buffer, sizeof(buffer), 1, file))){

      //Eliminamos la variable ret dado que no la necesitaremos como control dado que imprimimos por pantalla.

      //En fwrite pasamos por parametro el buffer, tamaño en bytes, numero de bytes, y el destino
	if(buffer != NULL){
	fwrite(buffer, sizeof(buffer), 1, stdout);

	}
	
      }
	printf("\n");
	free(buffer);
	fclose(file);
	return 0;
}
