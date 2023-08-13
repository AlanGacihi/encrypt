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

void encrypt(char *plaintext, char *key, char *ciphertext) {
    int plaintext_len = strlen(plaintext);
    int key_len = strlen(key);

    for (int i = 0; i < plaintext_len; i++) {
        if (plaintext[i] == ' ') {
            ciphertext[i] = ' '; // Preserve spaces
        } else {
            int plain_value = plaintext[i] - 'A'; // Convert to numerical value (A=0, B=1, ...)
            int key_value = key[i % key_len] - 'A'; // Repeating key

            int encrypted_value = (plain_value + key_value) % 27; // Perform encryption
            ciphertext[i] = encrypted_value + 'A'; // Convert back to character
        }
    }

    ciphertext[plaintext_len] = '\0'; // Null-terminate the ciphertext

    printf("%s\n", ciphertext);
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
    serv_addr.sin_addr.s_addr = INADDR_ANY;
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

            char plaintext[BUFFER_SIZE];
            char key[BUFFER_SIZE];
            char ciphertext[BUFFER_SIZE];

            memset(plaintext, 0, BUFFER_SIZE);
            memset(key, 0, BUFFER_SIZE);

            recv(newsockfd, plaintext, BUFFER_SIZE, 0);
            printf("[%d] Received message from client:\n%s\n", getpid(), plaintext);
            recv(newsockfd, key, BUFFER_SIZE, 0);
            printf("[%d] Received key from client:\n%s\n", getpid(), key);

            //encrypt(plaintext, key, ciphertext);

            //send(newsockfd, ciphertext, strlen(ciphertext), 0);

            close(newsockfd);
            exit(0);
        } else {
            close(newsockfd);
        }
    }

    close(sockfd);
    return 0;
}