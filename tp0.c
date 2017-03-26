#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
  output[1] = ((input[0] % 4) << 4) | input[1] >> 4;

  // i1: ZZZZYYYY, i2: XXZZZZZZ
  // o: 00YYYYXX
  output[2] = ((input[1] % 16) << 2) | input[2] >> 6;

  // i2: ZZXXXXXX
  // o: 00XXXXXX
  output[3] = input[2] % 64;
}

// @encoded_input are indexes that are 00XXXXXX, since there is no
// available 6-bit representation for these indexes.
void decode(char* encoded_input, char* output) {
  // i0: 00XXXXXX, i1: 00YYZZZZ
  // o: XXXXXXYY
  output[0] = (encoded_input[0] % 64) << 2 | encoded_input[1] >> 4;

  // i1: 00ZZYYYY, i2: 00XXXXZZ
  // o: YYYYXXXX
  output[1] = (encoded_input[1] % 16) << 4 | encoded_input[2] >> 2;

  // i2: 00ZZZZXX, i3: 00YYYYYY
  // o: XXYYYYYY
  output[2] = (encoded_input[2] % 4) << 6 | encoded_input[3] % 64;
}

int main (int argc, char** argv) {

    option_t options = { stdin, stdout, false };
    parse_options(argc, argv, &options);

    char input[] = {'M', 'a', 'n', '\0'};
    char encoded_output[4] = {0};
    char decoded_input[4] = {0};

    printf("%s\n", input);

    for (int i = 0; i < 3; i++)
      printf("%d ", input[i]);
    printf("\n");

    encode(input, encoded_output);
    decode(encoded_output, decoded_input);

    for (int i = 0; i < 4; i++)
      printf("%c ", encode_table[(int) encoded_output[i]]);
    printf("\n");

    for (int i = 0; i < 4; i++)
      printf("%d ", decode_char(encode_table[(int) encoded_output[i]]));
    printf("\n");

    printf("%s\n", decoded_input);

    close_files(&options);
    return 0;
}
