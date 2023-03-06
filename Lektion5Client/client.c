#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "iknlib.h"

#define SIZE 256

// @brief Udskriver fejlbesked og terminerer program
// @param message Fejlbesked der udskrives til terminal
void error(const char* message) {
    perror(message);
    exit(1);
}

int main(int argc, char* argv[]) {

    int socketfd;                                   // client socket
    int portno = 9000;                              // server pot
    int n;                                          // retur parameter for sendto() og recvfrom()

    struct sockaddr_in server;                      // struct der indeholder server information     
    struct sockaddr_in from;                        // struct der indeholder information om afsender af data (server)
    socklen_t length = sizeof(struct sockaddr_in);  // indeholder størrelse af struct sockaddr_in                      

    char buffer[SIZE];

    // error handle - tjek korrekt antal parameter givet til program
    if(argc < 2) {
        error("Missing either IP or command ('U' or 'L')");
    }

    // opret client socket
    socketfd = socket(AF_INET,SOCK_DGRAM,0);
    if(socketfd < 0) {
        error("ERROR creating socket");
    }

    // opsæt server struct
    bzero((char *) &server, sizeof(server));             // nulstil strukt
    server.sin_family = AF_INET;                         // set server protokol til TCP
    server.sin_addr.s_addr = inet_addr(argv[1]);         // indlæs Server IP
    server.sin_port = htons(portno);                     // set server 

    // send data anmodning til server
    n = sendto(socketfd,argv[2],sizeof(argv[2]),0,(struct sockaddr*)&server,length);
    if(n <= 0) {
        error("ERROR sending data to server");
    }

    // læs svar fra server
    n = recvfrom(socketfd,buffer,SIZE,0,(struct sockaddr*)&from, &length);
    if(n <= 0) {
        error("ERROR recieving data from server");
    }

    // print svar fra server til terminal
    printf(buffer);

    // afslut program
    close(socketfd);
    return 0;
}