#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define MAX_CONNECTIONS 5

void error(const char *msg) {
    perror(msg);
    exit(1);
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

    listen(sockfd, MAX_CONNECTIONS);

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

            // Verify client identity (dec_client)
            // You'll need to implement this part
            
            // Receive ciphertext and key from client
            char ciphertext[BUFFER_SIZE];
            char key[BUFFER_SIZE];
            char plaintext[BUFFER_SIZE];

            memset(ciphertext, 0, BUFFER_SIZE);
            memset(key, 0, BUFFER_SIZE);
            
            // Receive ciphertext and key using recv() from newsockfd
            
            // Perform decryption
            decrypt(ciphertext, key, plaintext);
            
            // Send plaintext back to client
            // You'll need to use send() here
            
            close(newsockfd);
            exit(0);
        } else {
            close(newsockfd);
        }
    }

    close(sockfd);
    return 0;
}
