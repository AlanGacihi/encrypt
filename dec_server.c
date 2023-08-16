#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 80000

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void decrypt(char *ciphertext, char *key, char *decrypted_text) {
    int ciphertext_len = strlen(ciphertext);
    int key_len = strlen(key);

    for (int i = 0; i < ciphertext_len; i++) {
        if (ciphertext[i] == ' ') {
            decrypted_text[i] = ' '; // Preserve spaces
        } else {
            int encrypted_value = ciphertext[i] - 'A'; // Convert to numerical value
            int key_value;
            
            if (key[i % key_len] == ' ') {
                key_value = 0; // Treat space in key as zero
            } else {
                key_value = key[i % key_len] - 'A';
            }

            int decrypted_value = (encrypted_value - key_value + 26) % 26; // Perform decryption
            decrypted_text[i] = decrypted_value + 'A'; // Convert back to character
        }
    }

    decrypted_text[ciphertext_len] = '\0'; // Null-terminate the decrypted text
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
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");;
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

            // Receive the length of the data first
            size_t combined_data_length;
            recv(newsockfd, &combined_data_length, sizeof(size_t), 0);

            // Create a buffer to receive data
            char received_data[combined_data_length];
            memset(received_data, 0, combined_data_length);

            // Receive data in a loop until all expected data is received
            size_t totalReceived = 0;
            while (totalReceived < combined_data_length) {
                ssize_t bytesReceived = recv(newsockfd, received_data + totalReceived, combined_data_length - totalReceived, 0);
                if (bytesReceived <= 0) {
                    // Handle error or connection closure
                    break;
                }
                totalReceived += bytesReceived;
            }

            // Split the received data at the full stop
            char *separator = strchr(received_data, '.');
            if (separator == NULL) {
                fprintf(stderr, "[%d] Invalid received data format.\n", getpid());
                close(newsockfd);
                exit(1);
            }

            // Calculate the lengths of ciphertext and key
            size_t ciphertext_length = separator - received_data;
            size_t key_length = strlen(separator + 1);

            // Extract ciphertext and key
            strncpy(ciphertext, received_data, ciphertext_length);
            ciphertext[ciphertext_length] = '\0'; // Null-terminate the plaintext
            strncpy(key, separator + 1, key_length);
            key[key_length] = '\0'; // Null-terminate the key

            decrypt(ciphertext, key, plaintext);
            send(newsockfd, plaintext, strlen(plaintext), 0);

            close(newsockfd);
            exit(0);

        } else {
            close(newsockfd);
        }

    }

    close(sockfd);
    return 0;
}