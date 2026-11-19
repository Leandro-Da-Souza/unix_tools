#include "G_H.h"
#include <stdbool.h>

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

  if (gArgs->size == 0) {
    return false;
  }

  return true;
}

char *toLowercase(char *str) {
  size_t i = 0;
  char *lower = malloc((strlen(str) + 1));

  if (lower == NULL) {
    return NULL;
  }

  while (true) {
    lower[i] = tolower((unsigned char)str[i]);
    if (str[i] == '\0')
      break;
    i++;
  }

  return lower;
}

bool readAndWrite(Arguments *gArgs) {
  char *line = NULL;
  size_t memsize = 0;
  ssize_t bytesread;
  bool fileError = false;
  char *lowerPattern = NULL;

  if (gArgs->flags.CASE_INSENSITIVE) {
    lowerPattern = toLowercase(gArgs->pattern);
    if (lowerPattern == NULL) {
      return false;
    }
  }

  for (int i = 0; i < gArgs->size; i++) {
    FILE *fstream = fopen(gArgs->files[i], "r");

    if (fstream == NULL) {
      perror("File could not be opened.\n");
      fileError = true;
      continue;
    }

    while ((bytesread = getline(&line, &memsize, fstream)) != -1) {
      if (gArgs->flags.CASE_INSENSITIVE && lowerPattern != NULL) {
        char *lowerLine = toLowercase(line);
        if (lowerLine == NULL) {
          fp("Could not read line, exiting.\n");
          fileError = true;
          break;
        }
        if (strstr(lowerLine, lowerPattern)) {
          p("[%s] %s", gArgs->files[i], line);
        }
        free(lowerLine);
      } else {
        if (strstr(line, gArgs->pattern)) {
          p("[%s] %s", gArgs->files[i], line);
        }
      }
    }

    if (ferror(fstream)) {
      perror("Failed to read file");
      fileError = true;
    }

    if (fclose(fstream) != 0) {
      perror("File could not be closed");
      fileError = true;
      continue;
    }
  }

  free(line);
  free(lowerPattern);

  return !fileError;
}

int main(int argc, char *argv[]) {
  int EXIT_STATUS = EXIT_SUCCESS;

  if (argc < 2) {
    fp("Not enough arguments passed.\n");
    EXIT_STATUS = EXIT_FAILURE;
    goto quit;
  }

  Arguments gArgs = {
      0, {false}, NULL, malloc((argc - 1) * sizeof *gArgs.files)};

  if (gArgs.files == NULL) {
    EXIT_STATUS = EXIT_FAILURE;
    goto quit;
  }

  if (parse(argc, argv, &gArgs) == false) {
    EXIT_STATUS = EXIT_FAILURE;
    goto cleanup;
  }

  if (readAndWrite(&gArgs) == false) {
    EXIT_STATUS = EXIT_FAILURE;
  };

cleanup:
  free(gArgs.files);
quit:
  return EXIT_STATUS;
}
