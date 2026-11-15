#include "G_H.h"

int EXIT_CODE;

typedef struct {
  bool COUNT_BYTES;
  bool COUNT_WORDS;
  bool COUNT_LINES;
} Flags;

typedef struct {
  int size;
  char **items;
} Files;

typedef struct {
  size_t bytes;
  size_t words;
  size_t lines;
} Count;

bool count(FILE *file, Count *c, Flags flag) {
  int i;
  bool insideWord = false;

  while ((i = fgetc(file)) != EOF) {
    if (flag.COUNT_BYTES)
      c->bytes++;
    if (flag.COUNT_LINES)
      if (i == '\n')
        c->lines++;
    if (flag.COUNT_WORDS) {
      if (!isspace(i) && !insideWord) {
        insideWord = true;
        c->words++;
      } else if (isspace(i) && insideWord) {
        insideWord = false;
      }
    }
  }

  if (ferror(file))
    return false;
  return true;
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

    Flags flags = {false, false, false};
    Files files = {0, malloc((argc - 1) * sizeof(*files.items))};

    if (files.items == NULL) {
      fp("Failed to allocate memory, exiting.\n");
      EXIT_CODE = 1;
      goto exit;
    }

    parse(argc, argv, &flags, &files);

    if (!flags.COUNT_BYTES && !flags.COUNT_WORDS && !flags.COUNT_LINES) {
      /* all are false then switch them to true to print EVERYTHING*/
      flags = (Flags){true, true, true};
    }
    for (int j = 0; j < files.size; j++) {

      Count result = {0, 0, 0};
      fileError = 0;
      fname = files.items[j];
      f = fopen(fname, "r");

      if (f == NULL) {
        perror(fname);
        EXIT_CODE = 1;
        continue;
      }

      if (count(f, &result, flags) == false) {
        perror(fname);
        EXIT_CODE = 1;
        fileError = 1;
      };

      if (fclose(f) != 0) {
        perror(fname);
        EXIT_CODE = 1;
        fileError = 1;
      };

      if (fileError == 0) {
        p("%s\n", fname);

        if (flags.COUNT_BYTES)
          p("bytes: %zu\n", result.bytes);

        if (flags.COUNT_WORDS)
          p("words: %zu\n", result.words);

        if (flags.COUNT_LINES)
          p("lines: %zu\n", result.lines);
      }
    }

    free(files.items);
  }

exit:
  return EXIT_CODE;
}
