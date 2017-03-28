#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "argv_parser.h"

char encode_table[64] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '+', '/'
};

// This conversion is based on ASCII table.
// @base64_symbol is a symbol that appears in @encode_table
char decode_char(char base64_symbol) {
  const char BASE_64_INDEX_a = 26;
  const char BASE_64_INDEX_0 = 52;

  if (base64_symbol >= 'a') {
    return base64_symbol - 'a' + BASE_64_INDEX_a;
  }
  if (base64_symbol >= 'A') {
    return base64_symbol - 'A';
  }
  if (base64_symbol >= '0') {
    return base64_symbol - '0' + BASE_64_INDEX_0;
  }

  return 62 + base64_symbol == '/';
}

// In order to take last n bits from a variable, we simply do: var % 2^n
// Obtained indexes are 00XXXXXX, since there is no 6-bit representation
void encode(char* input, char* output) {
  // i0: XXXXXXZZ
  // o: 00XXXXXX
  output[0] = input[0] >> 2;

  // i0: ZZZZZZXX, i1: YYYYZZZZ
  // o: 00XXYYYY
  output[1] = ((input[0] % 4) << 4) | (input[1] >> 4);

  // i1: ZZZZYYYY, i2: XXZZZZZZ
  // o: 00YYYYXX
  output[2] = ((input[1] % 16) << 2) | (input[2] >> 6);

  // i2: ZZXXXXXX
  // o: 00XXXXXX
  output[3] = input[2] % 64;

  output[0] = encode_table[(int) output[0]];
  output[1] = encode_table[(int) output[1]];
  output[2] = encode_table[(int) output[2]];
  output[3] = encode_table[(int) output[3]];
}

// @encoded_input are indexes that are 00XXXXXX, since there is no
// available 6-bit representation for these indexes.
void decode(char* encoded_input, char* output) {

  encoded_input[0] = decode_char(encoded_input[0]);
  encoded_input[1] = decode_char(encoded_input[1]);
  encoded_input[2] = decode_char(encoded_input[2]);
  encoded_input[3] = decode_char(encoded_input[3]);

  // i0: 00XXXXXX, i1: 00YYZZZZ
  // o: XXXXXXYY
  output[0] = (encoded_input[0] % 64) << 2 | (encoded_input[1] >> 4);

  // i1: 00ZZYYYY, i2: 00XXXXZZ
  // o: YYYYXXXX
  output[1] = (encoded_input[1] % 16) << 4 | (encoded_input[2] >> 2);

  // i2: 00ZZZZXX, i3: 00YYYYYY
  // o: XXYYYYYY
  output[2] = (encoded_input[2] % 4) << 6 | (encoded_input[3] % 64);
}

int read_bytes(int input_fd, char* buffer, int bytes) {
  while (read(input_fd, buffer, 1) && --bytes) buffer++;
  return bytes;
}

ssize_t write_bytes(int output_fd, char* buffer, int bytes) {
  return write(output_fd, buffer, bytes);
}

int main (int argc, char** argv) {

    option_t options = { STDIN_FILENO, STDOUT_FILENO, false };
    parse_options(argc, argv, &options);

    char padding_char = '=';

    void (*actions[2])(char*, char*) = { encode, decode };

    int bytes_to_complete_buffer = 0;
    int max_bytes_to_read = 3 + options.should_decode;
    int max_bytes_to_write = 3 + !options.should_decode;

    while (!bytes_to_complete_buffer) {
      char input[4] = {0};
      char output[4] = {0};
      bytes_to_complete_buffer = read_bytes(options.input_file_descriptor, input, max_bytes_to_read);

      if (bytes_to_complete_buffer == max_bytes_to_read) {
        bytes_to_complete_buffer = 0;
        break;
      }

      actions[options.should_decode](input, output);
      write_bytes(options.output_file_descriptor, output, max_bytes_to_write - bytes_to_complete_buffer);
    }

    while (bytes_to_complete_buffer--) write(options.output_file_descriptor, &padding_char, 1);

    close_files(&options);
    return 0;
}
