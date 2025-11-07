#include "file_reader.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void read_from_file(const char *filename) {
  int buffer[BLOCK_SIZE];
  size_t bytes_read;

  // defining a file pointer :
  FILE *file_target;

  file_target = fopen(filename, "rb");
  if (file_target == NULL) {
    perror("could not open the file target");
    return;
  }

  printf("Reading from file (block size: %d bytes)\n", BLOCK_SIZE);

  while ((bytes_read = fread(buffer, 1, BLOCK_SIZE, file_target)) > 0) {
    for (size_t i = 0; i < bytes_read; i++) {
      printf("%c\n", buffer[i]);
    }
    printf("\n");
  }
  fclose(file_target);
}
