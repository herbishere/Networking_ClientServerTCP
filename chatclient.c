/**
 * Programmer Name: Herbert Diaz <diazh@oregonstate.edu>
 * Program Name:    Program 1 - Chat Client
 * Program Description:
 *      Usage: chatclient [serverhost] [serverport]
 *      This program serves as the client for a messaging system
 *      between a client and server using the TCP protocol. After
 *      getting the user's handle, the user can send messages
 *      until either they or the host terminate the connection with
 *      the "\quit" command.
 * Course Name:     CS 372 Intro to Computer Networks
 * Last Modified:   02/05/2020
**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define MAX_CHARACTERS 500
#define HANDLE_SIZE 11

int main(int argc, char *argv[])
{
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;
    char buffer[MAX_CHARACTERS];

    if (argc < 3) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

    return 0;
}