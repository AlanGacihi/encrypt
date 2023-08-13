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

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting to enc_server");

    // Send plaintext and key to enc_server
    // Use send() here to send plaintext and key to the server

    // Receive ciphertext from enc_server
    char ciphertext[BUFFER_SIZE];
    memset(ciphertext, 0, BUFFER_SIZE);
    // Use recv() here to receive ciphertext from the server

    // Output ciphertext to stdout
    printf("%s\n", ciphertext);

    close(sockfd);
    return 0;
}

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

             // Receive plaintext and key from client
            char plaintext[BUFFER_SIZE];
            char key[BUFFER_SIZE];
            char ciphertext[BUFFER_SIZE];

            memset(plaintext, 0, BUFFER_SIZE);
            memset(key, 0, BUFFER_SIZE);
            
            // Receive plaintext and key using recv() from newsockfd
            
            // Perform encryption
            encrypt(plaintext, key, ciphertext);
            
            // Send ciphertext back to client
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
