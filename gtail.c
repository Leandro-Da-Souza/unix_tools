#include "G_H.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_CAPACITY 10

int EXIT_STATUS = 0;

typedef struct {
  size_t size;
  char *items[MAX_CAPACITY];
} CircularBuffer;

int main(int argc, char *argv[]) {

  if (argc < 2) {
    fp("Not enough arguments passed\n");
    EXIT_STATUS = 1;
    goto exit;
  }

  char *fName = argv[1];
  FILE *fStream = NULL;
  char *line = NULL;

  size_t mem = 0;

  if ((fStream = fopen(fName, "r")) == NULL) {
    fp("could not open file\n");
    EXIT_STATUS = 1;
    goto exit;
  }

  CircularBuffer cBuf = {MAX_CAPACITY, NULL};
  size_t lineIndex = 0;
  char *copy = NULL;

  while ((getline(&line, &mem, fStream)) != EOF) {
    // getline allocates one memory address to line, therefor we need to copy
    // the current value of line before we insert it, otherwise all line point
    // to the same memory adress and therefor reads the same line
    size_t index = lineIndex % cBuf.size;

    free(cBuf.items[index]);
    cBuf.items[index] = strdup(line);
    if (cBuf.items[index] == NULL) {
      fp("String duplication failed, exiting.\n");
      EXIT_STATUS = 1;
      goto cleanup;
    }
    lineIndex++;
  }

  if (ferror(fStream)) {
    fp("Could not read file\n");
    EXIT_STATUS = 1;
    goto cleanup;
  }

  for (int i = 0; i < MAX_CAPACITY; i++) {
    p("%s\n", cBuf.items[i]);
  }

cleanup:
  if (fStream != NULL && fclose(fStream) != 0) {
    fp("Could not close file\n");
    EXIT_STATUS = 1;
  }

  for (int i = 0; i < MAX_CAPACITY; i++) {
    if (cBuf.items[i] != NULL) {
      free(cBuf.items[i]);
    }
  }
  free(line);
exit:
  return EXIT_STATUS;
}
