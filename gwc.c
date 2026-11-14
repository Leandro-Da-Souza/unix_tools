#include "g.h"

int count(FILE *file) {
  int c = 0;
  while (fgetc(file) != EOF) {
    c++;
  }

  if (ferror(file))
    return -1;
  return c;
}

int main(int argc, char *argv[]) {
  int EXIT_CODE = 0;
  int COUNT;

  if (argc > 1) {
    FILE *f;
    char *fname;
    int fileError;

    for (int j = 1; j < argc; j++) {
      COUNT = 0;
      fileError = 0;
      fname = argv[j];
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
  }

  return EXIT_CODE;
}
