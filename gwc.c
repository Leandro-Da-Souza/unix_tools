#include <stdio.h>

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
  int COUNT = 0;

  if (argc > 1) {
    FILE *f;
    char *fname;

    for (int j = 1; j < argc; j++) {
      fname = argv[1];
      f = fopen(fname, "r");

      if (f == NULL) {
        perror(fname);
        EXIT_CODE = 1;
        break;
      }

      if ((COUNT = count(f)) == -1) {
        perror(fname);
        EXIT_CODE = 1;
      };

      if (fclose(f) != 0) {
        perror(fname);
        EXIT_CODE = 1;
      };
    }

    printf("%d", COUNT);
  }

  return EXIT_CODE;
}
