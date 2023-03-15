#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "iknlib.h"

// @brief Funktion til at udskrive fejlbeskeder til terminal
// @param msg Fejlbesked der ønskes udskrivet
void error(const char * msg) {
    perror(msg);
    exit(1);
}

 // @brief Modtager en fil fra server socket
 // @param serverSocket socket stream til server
 // @param fileName filnavn, muligvis med sti
 // @param fileSize størelse af fil
void receiveFile(int serverSocket, const char* fileName, long fileSize)
{
    FILE * fp;
    uint8_t buffer[1000];
    int dataToRead = fileSize;
    int numberOfBytes;

	printf("Receiving: '%s', size: %li\n", fileName, fileSize);

    fp = fopen(fileName,"wb");

    do {
        printf("Data: %li / %li\n",(fileSize-dataToRead), fileSize);

        bzero(buffer,sizeof(buffer));

        if(dataToRead < 1000 && dataToRead > 0) {
            numberOfBytes = read(serverSocket,buffer,dataToRead);
            fwrite(buffer,1,numberOfBytes,fp);
            printf("Debug data print small file: %s \n", buffer);
            printf("Data to read: %i\n" , dataToRead);
            printf("Number of bytes: %i\n" , numberOfBytes);
            dataToRead -= numberOfBytes;
        }
        else if(dataToRead >= 1000)
        {
            numberOfBytes = recv(serverSocket,buffer,sizeof(buffer),MSG_WAITALL);
            fwrite(buffer,1,numberOfBytes,fp);
            printf("Debug data print: %s \n", buffer);
            printf("Data to read: %i\n" , dataToRead);
            printf("Number of bytes: %i\n" , numberOfBytes);
            dataToRead -= numberOfBytes;
        }

    } while(numberOfBytes);

    fclose(fp);
    
}

int main(int argc, char* argv[]) {

    int sockfd;                     // server socket
    int portno;                     // port for server
    int n;                          // return parameter for write/read funktioner
    struct sockaddr_in serv_addr;   // strukt med server adresse/port, defineret i <netinet/in.h>
    struct hostent *server;         // strukt med server information, defineret i <netdb.h>
    char charBuffer[256];

    

    if (argc < 3) {     // tjek at server ip, port og fil er givet
        error("Missing Server IP or file name. Closing application....");
    }

    portno = 9000;                                 // hardcode portno
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);   // lav TCP socket
    if (sockfd < 0) {                           // error handle
        error("ERROR opening socket");
    }


    bzero((char *) &serv_addr, sizeof(serv_addr));          // nulstil strukt
    serv_addr.sin_family = AF_INET;                         // set server protokol til TCP
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);         // indlæs Server IP
    serv_addr.sin_port = htons(portno);                     // set server 

    if (connect(sockfd,&serv_addr,sizeof(serv_addr)) < 0) { // forbind til server
        error("ERROR connecting");
    }

    writeTextTCP(sockfd,argv[2]);                       // anmod om fil
    readTextTCP(sockfd,charBuffer,sizeof(charBuffer));  // læs filstørelse
    printf("File size: %s\n",charBuffer);
    int fileSize = atoi(charBuffer);                    // gem som integer
    receiveFile(sockfd,argv[2],fileSize);               // download fil

    return 0;
}
