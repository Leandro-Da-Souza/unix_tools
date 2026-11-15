#include "G_H.h"
#include <stdio.h>

int EXIT_CODE;

typedef struct {
  bool COUNT_BYTES;
  bool COUNT_WORDS;
  bool COUNT_LINES;
} Flags;

typedef struct {
  int size;
  int capacity;
  char **items;
} Files;

int count(FILE *file) {
  int c = 0;
  while (fgetc(file) != EOF) {
    c++;
  }

  if (ferror(file))
    return -1;
  return c;
}

void parse(int argc, char *argv[], Flags *flags, Files *files) {
  for (int j = 1; j < argc; j++) {
    if (strcmp(argv[j], "-c") == 0) {
      flags->COUNT_BYTES = true;
      continue;
    }

    if (strcmp(argv[j], "-w") == 0) {
      flags->COUNT_WORDS = true;
      continue;
    }

    if (strcmp(argv[j], "-l") == 0) {
      flags->COUNT_LINES = true;
      continue;
    }

    files->items[files->size] = argv[j];
    files->size++;
  }
}

int main(int argc, char *argv[]) {
  EXIT_CODE = 0;

  if (argc > 1) {
    FILE *f;
    char *fname;
    int fileError;
    int COUNT;

    Flags flags = {false, false, false};
    Files files = {0, (argc - 1), malloc((argc - 1) * sizeof(*files.items))};

    if (files.items == NULL) {
      printf("Failed to allocate memory, exiting.\n");
      EXIT_CODE = 1;
      goto exit;
    }

    parse(argc, argv, &flags, &files);

    for (int j = 0; j < files.size; j++) {
      COUNT = 0;
      fileError = 0;
      fname = files.items[j];
      f = fopen(fname, "r");

      if (f == NULL) {
        perror(fname);
        EXIT_CODE = 1;
        continue;
      }

      if ((COUNT = count(f)) == -1) {
        perror(fname);
        EXIT_CODE = 1;
        fileError = 1;
      };

      if (fclose(f) != 0) {
        perror(fname);
        EXIT_CODE = 1;
        fileError = 1;
      };

      if (COUNT != -1 && fileError == 0) {
        p("%d %s\n", COUNT, fname);
      }
    }

    free(files.items);
  }

exit:
  return EXIT_CODE;
}
