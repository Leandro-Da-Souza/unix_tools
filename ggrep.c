#include "G_H.h"

int main(int argc, char *argv[]) {
  int EXIT_STATUS = EXIT_SUCCESS;

  if (argc < 2) {
    EXIT_STATUS = EXIT_FAILURE;
    goto exit;
  }

  char *fname = argv[1];
  FILE *fstream = fopen(fname, "r");
  char *line = NULL;
  size_t size;
  ssize_t nread;

  if (fstream == NULL) {
    perror("File could not be opened.\n");
    EXIT_STATUS = EXIT_FAILURE;
  }

  while ((nread = getline(&line, &size, fstream) != -1)) {
    p("%s", line);
    p("%zu\n", nread);
    p("%zu\n", size);
  }

exit:
  p("");
  return EXIT_STATUS;
}
