#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 2048

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

    // Remove new line character from ciphertext
    ciphertext[strcspn(ciphertext, "\n")] = '\0';

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

    // Remove new line character from key
    key[strcspn(key, "\n")] = '\0';

    // Check for bad characters in ciphertext and key
    for (int i = 0; ciphertext[i] != '\0'; i++) {
        if ((ciphertext[i] < 'A' || ciphertext[i] > 'Z') && ciphertext[i] != ' ') {
            fprintf(stderr, "dec_client error: input contains bad characters\n");
            return 1;
        }
    }

    for (int i = 0; key[i] != '\0'; i++) {
        if ((key[i] < 'A' || key[i] > 'Z') && key[i] != ' ') {
            fprintf(stderr, "dec_client error: input contains bad characters\n");
            return 1;
        }
    }

    // Check if key is shorter than ciphertext
    if (strlen(key) < strlen(ciphertext)) {
        fprintf(stderr, "Error: Key \'%s\' is too short\n", ciphertext_filename);
        return 1;
    }

    // Combine ciphrtext and key with a full stop separator
    char combined_data[BUFFER_SIZE * 2 + 1];
    snprintf(combined_data, sizeof(combined_data), "%s.%s", ciphertext, key);

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

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "Error: could not contact dec_server on port %d.\n", port);
        return 2; // Exit with error code 2 as specified
    }

    // Send combined data to dec_server
    send(sockfd, combined_data, strlen(combined_data), 0);

    //Receive plaintext from dec_server
    char plaintext[BUFFER_SIZE];
    memset(plaintext, 0, BUFFER_SIZE);
    recv(sockfd, plaintext, BUFFER_SIZE, 0);

    // Output ciphertext to stdout
    printf("%s\n", plaintext);

    close(sockfd);
    return 0;
}