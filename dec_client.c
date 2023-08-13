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
        fprintf(stderr, "Usage: %s ciphertext key port\n", argv[0]);
        return 1;
    }

    char *ciphertext_filename = argv[1];
    char *key_filename = argv[2];
    int port = atoi(argv[3]);

    // Read ciphertext
    FILE *ciphertext_file = fopen(ciphertext_filename, "r");
    if (ciphertext_file == NULL) {
        fprintf(stderr, "Error opening ciphertext file.\n");
        return 1;
    }
    char ciphertext[BUFFER_SIZE];
    if (fgets(ciphertext, BUFFER_SIZE, ciphertext_file) == NULL) {
        fprintf(stderr, "Error reading ciphertext.\n");
        fclose(ciphertext_file);
        return 1;
    }
    fclose(ciphertext_file);

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

    // Connect to dec_server
    int sockfd;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // localhost

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting to dec_server");

    // Send ciphertext and key to dec_server
    // Use send() here to send ciphertext and key to the server

    // Receive plaintext from dec_server
    char plaintext[BUFFER_SIZE];
    memset(plaintext, 0, BUFFER_SIZE);
    // Use recv() here to receive plaintext from the server

    // Output plaintext to stdout
    printf("%s\n", plaintext);

    close(sockfd);
    return 0;
}