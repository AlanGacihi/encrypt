#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s plaintext key port\n", argv[0]);
        return 1;
    }

    char *plaintext_filename = argv[1];
    char *key_filename = argv[2];
    int port = atoi(argv[3]);

    // Read plaintext
    FILE *plaintext_file = fopen(plaintext_filename, "r");
    if (plaintext_file == NULL) {
        fprintf(stderr, "Error opening plaintext file.\n");
        return 1;
    }
    char plaintext[BUFFER_SIZE];
    if (fgets(plaintext, BUFFER_SIZE, plaintext_file) == NULL) {
        fprintf(stderr, "Error reading plaintext.\n");
        fclose(plaintext_file);
        return 1;
    }
    fclose(plaintext_file);

    // Read key
    FILE *key_file = fopen(key_filename, "r");
    if (key_file == NULL) {
        fprintf(stderr, "Error opening key file.\n");
        return 1;
    }
    char key[BUFFER_SIZE];
    if (fgets(key, BUFFER_SIZE, key_file) == NULL) {
        fprintf(stderr, "Error reading key.\n");
        fclose(key_file);
        return 1;
    }
    fclose(key_file);

    // Check for bad characters in plaintext and key
    for (int i = 0; plaintext[i] != '\0'; i++) {
        if ((plaintext[i] < 'A' || plaintext[i] > 'Z') && plaintext[i] != ' ') {
            fprintf(stderr, "Error: Invalid character in plaintext.\n");
            return 1;
        }
    }

    for (int i = 0; key[i] != '\0'; i++) {
        if (key[i] < 'A' || key[i] > 'Z') {
            fprintf(stderr, "Error: Invalid character in key.\n");
            return 1;
        }
    }

    // Check if key is shorter than plaintext
    if (strlen(key) < strlen(plaintext)) {
        fprintf(stderr, "Error: Key \'%s\' is too short\n", plaintext_filename);
        return 1;
    }

    // Connect to enc_server
    int sockfd;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // localhost

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "Error connecting to enc_server on port %d.\n", port);
        return 2; // Exit with error code 2 as specified
    }

    // Send plaintext and key to enc_server
    send(sockfd, plaintext, strlen(plaintext), 0);
    send(sockfd, key, strlen(key), 0);

    // Receive ciphertext from enc_server
    char ciphertext[BUFFER_SIZE];
    memset(ciphertext, 0, BUFFER_SIZE);
    recv(sockfd, ciphertext, BUFFER_SIZE, 0);

    // Output ciphertext to stdout
    printf("%s\n", ciphertext);

    close(sockfd);
    return 0;
}