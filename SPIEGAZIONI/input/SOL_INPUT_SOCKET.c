#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char data[4] = {0xde, 0xad, 0xbe, 0xef}; // Example 4 bytes of data

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = 28695; // Replace with your local port
    server_addr.sin_addr.s_addr = INADDR_ANY; // Replace with your local IP if different

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (send(sockfd, data, sizeof(data), 0) < 0) {
        perror("Send failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("4 bytes of data sent successfully\n");

    close(sockfd);

    return 0;
}
// Per far funzionare il programma bisogna creare un link simbolico all'interno della cartella dove si é creato il file che contiene tutti gli input da mandare
