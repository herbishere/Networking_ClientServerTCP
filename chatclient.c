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

#define MAX_CHARACTERS 501
#define MAX_HANDLE_SIZE 11
#define h_addr h_addr_list[0]

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int getInput(char* input, int inputSize)
{
    fgets(input, inputSize, stdin);
    return 0;
}

int sendMessage(char* handle, char* message, int fileDescriptor);

int checkQuit(char* buffer, char* exitCommand, int socketFD)
{
    if (!strcmp(buffer, exitCommand))
    {
        close(socketFD);
    }
}

int main(int argc, char *argv[])
{
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;
    char buffer[MAX_CHARACTERS], handle[MAX_HANDLE_SIZE];

    if (argc < 3) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

    // Set up server address struct
    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[2]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname(argv[1]); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

    // Create TCP Socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");

    // Connect to Server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

    // Get Handle From User
    printf("Enter Handle: ");
    getInput(handle, MAX_HANDLE_SIZE);
    int handleLength = strlen(handle);  // Get the Number of Characters of Handle
    handle[handleLength - 1] = 0;       // Remove Trailing Newline

    // Get Message To Send or Quit Program
    printf("%s> ", handle);
    getInput(buffer, MAX_CHARACTERS - handleLength);
    checkQuit(buffer, "\\quit", socketFD);

    // Send Message
    char message[MAX_CHARACTERS];
    snprintf(message, MAX_CHARACTERS, "%s> %s", handle, buffer);
    sendMessage(handle, message, socketFD);

    // Receive and Print Message to Server

    return 0;
}

/*********************************************************************
 * int sendMessage(char* source, char* message, int fileDescriptor)
 *  Sends a message to the connection.
 * Arguments:
 * 	char* handle - Whether the user's handle
 *  char* message - the message to send.
 *  int fileDescriptor - the file descriptor of the connection.
 * Returns:
 * 	0 on success.
*********************************************************************/
int sendMessage(char* handle, char* message, int fileDescriptor)
{
	int charsWritten;

	// printf("%s: I sent this to the server \"%s\" %ld\n", handle, message, strlen(message));
	charsWritten = send(fileDescriptor, message, strlen(message), 0); // Write to the server
	if (charsWritten < 0) { fprintf(stderr, "%s", handle); error(": ERROR writing to socket"); }
	if (charsWritten < strlen(message)) printf("%s: WARNING: Not all data written to socket!\n", handle);
	
	return 0;
}