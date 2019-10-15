#ifndef _MYTAR_H
#define _MYTAR_H

#include <limits.h>

typedef enum{
  NONE,
  ERROR,
  CREATE,
  EXTRACT,
  LIST,
  ADD,
  REMOVE
} flags;

typedef struct {
  char* name;
  unsigned int size;
} stHeaderEntry;

int createTar(int nFiles, char *fileNames[], char tarName[]);
int extractTar(char tarName[]);
int listTar(char tarName[]);
int addFileToTar(int nFiles, char *fileNames[], char tarName[], char newTarName[]);
int removeFileFromTar(int nFiles, char *fileNames[], char tarName[], char newTarName[]);

#endif /* _MYTAR_H */
