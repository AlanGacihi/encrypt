#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 70000

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void encrypt(char *plaintext, char *key, char *ciphertext) {
    int plaintext_len = strlen(plaintext);
    int key_len = strlen(key);

    for (int i = 0; i < plaintext_len; i++) {
        if (plaintext[i] == ' ') {
            ciphertext[i] = ' '; // Preserve spaces
        } else {
            int plain_value = plaintext[i] - 'A'; // Convert to numerical value (A=0, B=1, ...)
            int key_value;
            
            if (key[i % key_len] == ' ') {
                key_value = 0; // Treat space in key as zero
            } else {
                key_value = key[i % key_len] - 'A';
            }

            int encrypted_value = (plain_value + key_value) % 26; // Perform encryption
            ciphertext[i] = encrypted_value + 'A'; // Convert back to character
        }
    }

    ciphertext[plaintext_len] = '\0'; // Null-terminate the ciphertext
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s listening_port\n", argv[0]);
        return 1;
    }

    int listening_port = atoi(argv[1]);
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    portno = listening_port;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.2");;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);

    clilen = sizeof(cli_addr);
    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");

        int pid = fork();
        if (pid < 0)
            error("ERROR on fork");
        if (pid == 0) {
            // Child process
            close(sockfd);

            char combined_data[BUFFER_SIZE * 2];
            char plaintext[BUFFER_SIZE];
            char key[BUFFER_SIZE];
            char ciphertext[BUFFER_SIZE];

            memset(combined_data, 0, BUFFER_SIZE * 2);
            memset(plaintext, 0, BUFFER_SIZE);
            memset(key, 0, BUFFER_SIZE);

            recv(newsockfd, combined_data, BUFFER_SIZE * 2, 0);

            printf("COMBINED: %ld\n", combined_data);
            
            // Split the combined data at the full stop
            char *separator = strchr(combined_data, '.');
            if (separator == NULL) {
                fprintf(stderr, "[%d] Invalid combined data format.\n", getpid());
                close(newsockfd);
                exit(1);
            }

            // Calculate the lengths of plaintext and key
            size_t plaintext_length = separator - combined_data;
            size_t key_length = strlen(separator + 1);

            // Extract plaintext and key
            strncpy(plaintext, combined_data, plaintext_length);
            plaintext[plaintext_length] = '\0'; // Null-terminate the plaintext
            strncpy(key, separator + 1, key_length);
            key[key_length] = '\0'; // Null-terminate the key

            encrypt(plaintext, key, ciphertext);
            send(newsockfd, ciphertext, strlen(ciphertext), 0);

            close(newsockfd);
            exit(0);

        } else {
            close(newsockfd);
        }

    }

    close(sockfd);
    return 0;
}