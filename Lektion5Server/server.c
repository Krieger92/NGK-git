#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "iknlib.h"

#define SIZE 256

// @brief Udskriver fejlbesked og terminerer program
// @param message Fejlbesked der udskrives til terminal
void error(const char* message) {
    perror(message);
    exit(1);
}

// @brief Sender en fil fra server til client med UDP
// @param serverSocket Socket som data læses fra
// @param from Hvor data kommer fra (fra UDP header)
// @param fromlen Længden af sockaddr_in struct
// @param fileName Navn på fil server skal sende
void sendFile(const int serverSocket, struct sockaddr_in from, socklen_t fromlen, const char* fileName) {
    
    FILE* fp;
    uint8_t dataBuffer[SIZE];
    long n;
    
    // open fil
    fp = fopen(fileName,"rb");
    if(fp == NULL) {
        error("ERROR opening file");
    }

    // aflæs fil og gem i dataBuffer
    n = fread(dataBuffer,1,sizeof(dataBuffer),fp);
    if(n <= 0) {
        error("Error reading file");
    }

    // send dataBuffer til client
    n = sendto(serverSocket,dataBuffer,n,0,(struct sockaddr*)&from,fromlen);
    if(n <= 0) {
        error("Error sending data");
    }

    printf("Data request handled, waiting for new data request\n");
    fclose(fp);
}

int main(int argc, char * argv[]) {

    int socketfd;       // server socket
    int portno;         // server portnumber
    int n;              // retur parameter for read() og write()

                                                        // struct fra <netinet/in.h>
    struct sockaddr_in server;                          // server adresse/forbindelse info
    struct sockaddr_in from;                            // client adresse/forbindelse info
    socklen_t fromlen = sizeof(struct sockaddr_in);     // parameter i recvfrom()

    char buffer[SIZE];

    printf("Starting server...\n");

    if(argc < 2) {  // Hvis portNumber ikke er givet set til default
        portno = 9000;
    } else {
        portno = atoi(argv[1]);     // set portno til argument givet til main
    }
                                                // create server socket
    socketfd = socket(AF_INET,SOCK_DGRAM,0);    // use UDP 
    if(socketfd < 0) {                          // error handle
        error("ERROR - could not open server socket\n");
    }

    bzero((char *) &server, sizeof(server));  // set server adresse til 0

                                             // Initier server struct
    server.sin_family = AF_INET;             // set til IPv4
    server.sin_port = htons(portno);         // set portno
    server.sin_addr.s_addr = INADDR_ANY;     // set server IP (til host der kører program)

    if (bind(socketfd, (struct sockaddr *) &server, sizeof(server)) < 0) {  // bind socket
        error("ERROR on binding\n");                                                // error handle
    }

    printf("Server started, waiting for data request...\n");

    for(;;) {

        /* HØR CLIENT */
        n = recvfrom(socketfd,buffer,SIZE,0,(struct sockaddr *)&from,&fromlen);
        if(n < 0) {
            error("ERROR on reading socket\n");
        }
        else {
            printf("Got a data request from socket\n");
        }

        // lav til modtaget char til uppercase (undgå case-sensitivitet)
        for (size_t i = 0; i < n; i++) {
            buffer[i] = toupper(buffer[i]);
        }

        // Switch-Case håndterer data fra client
        switch ((int) buffer[0]) {
        
            case 'U':
                sendFile(socketfd,from,fromlen,"/proc/uptime");
                break;
        
            case 'L':
                sendFile(socketfd,from,fromlen,"/proc/loadavg");
                break;

            // Send fejlbesked og fortsæt program
            default:
                printf("ERROR 404 - Requested file not found, waiting for new data request...\n");
                n = sendto(socketfd,"ERROR 404 - Requested file not found\n",38,0,(struct sockaddr*)&from,fromlen);
                if(n <= 0) {
                    error("Error sending data");
                }
                break;
        }
        
    }

    close(socketfd);
    return 0;
}