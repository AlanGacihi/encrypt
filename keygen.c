#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s keylength\n", argv[0]);
        return 1;
    }

    int keylength = atoi(argv[1]);

    // Set the seed for random number generation
    srand(time(NULL));

    for (int i = 0; i < keylength; i++) {
        // Generate a random number between 0 and 26
        int random_char = rand() % 27;

        // Convert the number to a character
        char random_char_char;
        if (random_char < 26) {
            random_char_char = 'A' + random_char;
        } else {
            random_char_char = ' '; // Space character
        }

        // Output the random character
        putchar(random_char_char);
    }

    putchar('\n'); // Output a newline at the end

    return 0;
}
