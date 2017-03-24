#include <stdio.h>

int main (int argc, char** argv) {
    char input[] = {'M', 'a', 'n'};
    char encoded_output[4] = {0};

    for (int i = 0; i < 3; i++) printf("%d ", input[i]);

    // In order to take last n bits from a variable, we simply do: var % 2^n
    encoded_output[0] = input[0] >> 2;
    encoded_output[1] = ((input[0] % 4) << 4) | input[1] >> 4;
    encoded_output[2] = ((input[1] % 16) << 2) | input[2] >> 6;
    encoded_output[3] = input[2] % 64;

    for (int i = 0; i < 4; i++) printf("%d ", encoded_output[i]);
    printf("\n");

    return 0;
}
