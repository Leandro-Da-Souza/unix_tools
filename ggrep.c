#include "G_H.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  bool CASE_INSENSITIVE;
} Flags;

typedef struct {
  size_t size;
  Flags flags;
  char *pattern;
  char **files;
} Arguments;

bool parse(int argc, char *argv[], Arguments *gArgs) {
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if (strcmp(argv[i], "-i") == 0) {
        gArgs->flags.CASE_INSENSITIVE = true;
      } else {
        return false;
      }
      continue;
    }

    if (gArgs->pattern == NULL) {
      gArgs->pattern = argv[i];
      continue;
    }

    gArgs->files[gArgs->size] = argv[i];
    gArgs->size++;
  }

  if (gArgs->pattern == NULL) {
    return false;
  }

  return true;
}

int main(int argc, char *argv[]) {
  int EXIT_STATUS = EXIT_SUCCESS;

  if (argc < 2) {
    fp("Not enough arguments passed.\n");
    EXIT_STATUS = EXIT_FAILURE;
    goto exit;
  }

  Arguments gArgs = {
      0, {false}, NULL, malloc((argc - 1) * sizeof *gArgs.files)};

  if (parse(argc, argv, &gArgs) == false) {
    EXIT_STATUS = EXIT_FAILURE;
    goto exit;
  }

  FILE *fstream = fopen(*gArgs.files, "r");
  char *line = NULL;
  size_t memsize = 0;
  size_t bytesread;

  if (fstream == NULL) {
    perror("File could not be opened.\n");
    EXIT_STATUS = EXIT_FAILURE;
    goto exit;
  }

  while ((bytesread = getline(&line, &memsize, fstream)) != -1) {
    if (strstr(line, gArgs.pattern)) {
      p("found search \"%s\" in \"%s\" from %s\n", gArgs.pattern, line,
        *gArgs.files);
    }
  }

  if (fclose(fstream) != 0) {
    perror("File could not be closed");
    EXIT_STATUS = EXIT_FAILURE;
    goto exit;
  }
exit:
  free(line);
  free(gArgs.files);
  return EXIT_STATUS;
}
