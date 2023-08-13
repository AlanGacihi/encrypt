#include <stdio.h>
#include <string.h>

// Encrypt plaintext using key
void encrypt(char *plaintext, char *key, char *ciphertext) {
    int plaintext_len = strlen(plaintext);
    int key_len = strlen(key);

    for (int i = 0; i < plaintext_len; i++) {
        // Skip spaces in the plaintext
        if (plaintext[i] == ' ') {
            ciphertext[i] = ' ';
            continue;
        }

        // Calculate modular addition and store in ciphertext
        int plain_val = plaintext[i] - 'A'; // Convert to numerical value (A=0, B=1, ...)
        int key_val = key[i % key_len] - 'A';
        int cipher_val = (plain_val + key_val) % 27; // 26 letters + space
        ciphertext[i] = cipher_val + 'A'; // Convert back to character
    }
    ciphertext[plaintext_len] = '\0';
}

// Decrypt ciphertext using key
void decrypt(char *ciphertext, char *key, char *plaintext) {
    int ciphertext_len = strlen(ciphertext);
    int key_len = strlen(key);

    for (int i = 0; i < ciphertext_len; i++) {
        // Skip spaces in the ciphertext
        if (ciphertext[i] == ' ') {
            plaintext[i] = ' ';
            continue;
        }

        // Calculate modular subtraction and store in plaintext
        int cipher_val = ciphertext[i] - 'A'; // Convert to numerical value
        int key_val = key[i % key_len] - 'A';
        int plain_val = (cipher_val - key_val + 27) % 27; // Add 27 to ensure positive result
        plaintext[i] = plain_val + 'A'; // Convert back to character
    }
    plaintext[ciphertext_len] = '\0';
}

int main() {
    char key[] = "XMCKL"; // Use your key here
    char plaintext[] = "HELLO"; // Use your plaintext here
    char ciphertext[strlen(plaintext) + 1];
    char decrypted_text[strlen(plaintext) + 1];

    encrypt(plaintext, key, ciphertext);
    decrypt(ciphertext, key, decrypted_text);

    printf("Plaintext: %s\n", plaintext);
    printf("Ciphertext: %s\n", ciphertext);
    printf("Decrypted Text: %s\n", decrypted_text);

    return 0;
}