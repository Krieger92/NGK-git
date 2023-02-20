#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "iknlib.h"

// @brief Funktion til at udskrive fejlbeskeder til terminal
// @param msg Fejlbesked der ønskes udskrivet
void error(const char * msg) {
    perror(msg);
    exit(1);
}

 // @brief Receives a file from a server socket
 // @param serverSocket Socket stream to server
 // @param fileName Name of file. Might include path on server!
 //
void receiveFile(int serverSocket, const char* fileName, long fileSize)
{
	printf("Receiving: '%s', size: %li\n", fileName, fileSize);

}

