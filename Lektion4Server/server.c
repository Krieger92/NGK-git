#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "iknlib.h"

#define SIZE 255

// @brief Funktion til at udskrive fejlbeskeder til terminal
// @param msg Fejlbesked der ønskes udskrivet
void error(const char * msg) {
    perror(msg);
    exit(1);
}

// @brief Funktion til at sende en fil fra server til client
// @param clientSocket Clientens socket (newsocketfd)
// @param fileName Navn på filen der ønskes sendt
// @param fileSize Størelsen på filen der ønskes sendt
void sendFile(const int clientSocket, const char* fileName, long fileSize)
{
    FILE * fp;                      // pointer til fil 
    size_t numberOfBytes = 0;      // retur parameter for fread (antal bytes læst)
    uint8_t dataBuffer[1000];       // buffer med plads til 1000 bytes  
    char charBuffer[256];
    long dataToSend = fileSize;     // Antal bytes der mangler at blive sendt

	printf("Sending: %s, size: %li\n", fileName, fileSize);     // opdater terminal
    
    snprintf(charBuffer,sizeof(charBuffer),"%li",fileSize);
    write(clientSocket,charBuffer,sizeof(charBuffer));

    fp = fopen(fileName,"rb");      // open fil

    do 
    {                                                                  // imens der stadig er mere at læse
        bzero(dataBuffer,sizeof(dataBuffer));                        // hvis der er mindre end 1000 bytes tilbage
        numberOfBytes = fread(dataBuffer,1,dataToSend,fp);          // læs resten af filen
        write(clientSocket,dataBuffer,numberOfBytes);                   // skriv op til 1000 bytes til bruger
        printf("Debug data print: %s \n", dataBuffer);
        dataToSend -= numberOfBytes;                                    // opdater antal bytes der mangler at blive sendt
    } while (numberOfBytes);

    printf("Data: %li / %li\n",(fileSize-dataToSend), fileSize);   // opdater terminal

    fclose(fp);
	
}

// @brief Server applikation - sender fil til client, hvis filen findes
// @param portno Serverens port nummer, som default 9000
int main(int argc, char* argv[]) {

    int socketfd;       // server socket
    int portno;         // server portnumber
    int newsocketfd;    // client socket
    int clilen;         // client addresse
    int n;              // retur parameter for read() og write()

    uint8_t buffer[SIZE];  // buffer 

                                    // struct fra <netinet/in.h>
    struct sockaddr_in serv_addr;   // server adresse/forbindelse info
    struct sockaddr_in cli_addr;    // client adresse/forbindelse info

    printf("Starting server...\n");

    if(argc < 2) {  // Hvis portNumber ikke er givet set til default
        perror("ERROR - no server port provided, default is 9000");
        portno = 9000;
    } else {
        portno = atoi(argv[1]);     // set portno til argument givet til main
    }
                                                // create server socket
    socketfd = socket(AF_INET,SOCK_STREAM,0);   // use TCP 
    if(socketfd < 0) {                          // error handle
        error("ERROR - could not open server socket");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));  // set server adresse til 0

                                                // Initier server struct
    serv_addr.sin_family = AF_INET;             // set til TCP
    serv_addr.sin_port = htons(portno);         // set portno
    serv_addr.sin_addr.s_addr = INADDR_ANY;     // set server IP (til host der kører program)

    if (bind(socketfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {  // bind socket
        error("ERROR on binding");                                                // error handle
    }

    listen(socketfd,5);     // Lyt efter client, kan have 5 i kø
    clilen = sizeof(cli_addr);

    printf("Listening after clients...\n");

    for(;;) {               // start loop 
        
        newsocketfd = accept(socketfd, (struct sockaddr *) &cli_addr, &clilen);     // vent på client forbinder
        if (newsocketfd < 0) {                                                      // error handle
            error("ERROR on accept");       
        }

        printf("Found client...\n");

        /* HØR CLIENT */
        readTextTCP(newsocketfd,buffer,sizeof(buffer));     // aflæs besked fra client
        const char* fileName = extractFileName(buffer);     // gem navn (e.g. test.png)
        const char* fileDirectory = buffer;                 // gem sti + navn (e.g. Desktop/NGK/test.png)
        long fileSize = getFilesize(fileName);              // check filstørelse
        if(fileSize < 0) {                                  // error handle
            writeTextTCP(newsocketfd,"Error 404 - file could not be found");
        }

        /* SVAR CLIENT*/
        sendFile(newsocketfd,fileName,fileSize);        // Opdater terminal m/ ønsket fil og data sent
        close(newsocketfd);
        printf("File sent! terminating connection with client...\n");

    }                       // slut loop

    close(socketfd);
    return 0;

}