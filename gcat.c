#include <stdbool.h>
#include <stdio.h>

bool readAndPrint(FILE *file) {
  int c;

  while ((c = fgetc(file)) != EOF) {
    if (putchar(c) == EOF) {
      return false;
    }
  }

  if (ferror(file)) {
    return false;
  }
  return true;
}

int main(int argc, char *argv[]) {
  int status = 0;
  char *fileName;
  FILE *file;

  if (argc > 1) {
    for (int j = 1; j < argc; j++) {
      fileName = argv[j];
      file = fopen(fileName, "r");

      /* File not found, continue to next file in argv*/
      if (file == NULL) {
        perror(fileName);
        status = 1;
        continue;
      }

      if (!readAndPrint(file)) {
        perror(fileName);
        status = 1;
      }

      /* Error occured while closing the file */
      if (fclose(file) != 0) {
        perror(fileName);
        status = 1;
      }
    }

  } else {
    if (!readAndPrint(stdin)) {
      fprintf(stderr, "gcat: error reading standard input\n");
      status = 1;
    }
  }
  return status;
}
