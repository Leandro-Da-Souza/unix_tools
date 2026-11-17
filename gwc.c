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

bool parse(int argc, char *argv[], Flags *flags, Files *files) {
  for (int j = 1; j < argc; j++) {
    // check for arguments, otherwise we are checking files
    if (argv[j][0] == '-' && argv[j][1] != '\0') {
      // long options aka --lines, --bytes etc
      if (argv[j][1] == '-') {
        if (strcmp(argv[j], "--bytes") == 0) {
          flags->COUNT_BYTES = true;
          continue;
        } else if (strcmp(argv[j], "--words") == 0) {
          flags->COUNT_WORDS = true;
          continue;
        } else if (strcmp(argv[j], "--lines") == 0) {
          flags->COUNT_LINES = true;
          continue;
        } else {
          fp("no valid option. \n");
          return false;
        }
      } else {
        // short options and combined flags
        for (int k = 1; argv[j][k] != '\0'; k++) {
          if (argv[j][k] == 'c') {
            flags->COUNT_BYTES = true;
            continue;
          } else if (argv[j][k] == 'w') {
            flags->COUNT_WORDS = true;
            continue;
          } else if (argv[j][k] == 'l') {
            flags->COUNT_LINES = true;
            continue;
          } else {
            fp("No valid option.\n");
            return false;
          }
        }
      }
    } else {
      files->items[files->size] = argv[j];
      files->size++;
    }
  }
  return true;
}

void printResult(Count *result, Flags flags, char name[]) {
  p("%s\n", name);

  if (flags.COUNT_BYTES)
    p("bytes: %zu\n", result->bytes);

  if (flags.COUNT_WORDS)
    p("words: %zu\n", result->words);

  if (flags.COUNT_LINES)
    p("lines: %zu\n", result->lines);
}

int main(int argc, char *argv[]) {
  EXIT_CODE = 0;
  FILE *f;
  char *fname;
  int fileError;
  Count result = {0, 0, 0};
  Flags flags = {false, false, false};
  Files files = {0,
                 (argc == 1 ? 0 : malloc((argc - 1) * sizeof(*files.items)))};

  if (argc > 1 && files.items == NULL) {
    fp("Failed to allocate memory, exiting.\n");
    EXIT_CODE = 1;
    goto exit;
  }

  if (!parse(argc, argv, &flags, &files)) {
    fp("Failed to parse files | arguments. \n");
    EXIT_CODE = 1;
    goto exit;
  };

  if (!flags.COUNT_BYTES && !flags.COUNT_WORDS && !flags.COUNT_LINES) {
    /* all are false then switch them to true to print EVERYTHING*/
    flags = (Flags){true, true, true};
  }

  if (files.size == 0) {
    // read stdin if no files have been parsed then exit
    if (count(stdin, &result, flags) == false) {
      perror("stdin");
      EXIT_CODE = 1;
      goto exit;
    } else {
      printResult(&result, flags, "stdin");
    };
    goto exit;
  }

  for (int j = 0; j < files.size; j++) {
    result = (Count){0, 0, 0};
    fileError = 0;
    fname = files.items[j];
    if (strcmp(fname, "-") == 0) {
      f = stdin;
    } else {
      f = fopen(fname, "r");
    }

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

    if (f != stdin) {
      if (fclose(f) != 0) {
        perror(fname);
        EXIT_CODE = 1;
        fileError = 1;
      };
    }

    if (fileError == 0) {
      printResult(&result, flags, fname);
    }
  }

exit:
  free(files.items);
  return EXIT_CODE;
}
