#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdbool.h>

typedef struct {
  FILE* input_file;
  FILE* output_file;
  bool should_decode;
} option_t;

void parse_options(int argc, char** argv, option_t* options);
void close_files(option_t* options);

#endif
