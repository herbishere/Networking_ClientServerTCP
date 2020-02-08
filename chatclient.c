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
 *      Code Adapted From:
 *          Beej
 *              > https://beej.us/guide/bgnet/html/
 *          CS344 Operating Systems
 *              > Instructor: Tyler Lawson
 *              > Program 4 
 * Course Name:     CS 372 Intro to Computer Networks
 * Last Modified:   02/07/2020
**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define MAX_CHARACTERS 500  // Max Characters To Send to Server
#define MAX_HANDLE_SIZE 10  // Max Characters of Handle
#define NUM_ARGS 3          // Number of Arguments for Function
#define h_addr h_addr_list[0]

// PROTOTYPES
void error(const char *msg);
void checkArguments(int argc, char* functionName);
int setupServerAddress(struct sockaddr_in* serverAddress, char* argv[]);
int createTCPSocket(int* socketFD);
int connectToServer(int socketFD, struct sockaddr_in* serverAddress);
int getInput(char* input, int inputSize);
int sendMessage(char* message, int fileDescriptor);
int recvMessage(char* message, int fileDescriptor);
int checkQuit(char* buffer, char* exitCommand, int socketFD);
void quit(int socketFD);

int main(int argc, char *argv[])
{
    int socketFD, handleLength;
    struct sockaddr_in serverAddress;
    char buffer[MAX_CHARACTERS + 1], handle[MAX_HANDLE_SIZE + 1];

    checkArguments(argc, argv[0]);              // Check usage & args

    // Initiate Contact
    setupServerAddress(&serverAddress, argv);   // Set up server address struct
    createTCPSocket(&socketFD);                 // Create TCP Socket
    connectToServer(socketFD, &serverAddress);  // Connect to Server

    // Get Handle From User
    printf("Enter Handle: ");               // Inform User to Enter Handle
    getInput(handle, MAX_HANDLE_SIZE + 1);  // Get User's Handle
    handleLength = strlen(handle);          // Get the Number of Characters of Handle
    
    while (1)
    {
        // Get Message To Send or Quit Program
        printf("%s> ", handle);
        getInput(buffer, MAX_CHARACTERS + 1 - handleLength);
        checkQuit(buffer, "\\quit", socketFD);

        // Send Message
        char message[MAX_CHARACTERS + 1];
        snprintf(message, MAX_CHARACTERS + 1, "%s> %s", handle, buffer);
        sendMessage(message, socketFD);

        // Receive Message from Server
        recvMessage(buffer, socketFD);

        // Print Message
        printf("%s\n", buffer);
    }

    return 0;
}

/*********************************************************************
*********************************************************************/
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

/*********************************************************************
*********************************************************************/
void checkArguments(int argc, char* functionName)
{
    if (argc < NUM_ARGS)
    {
        fprintf(stderr,"USAGE: %s hostname port\n", functionName);
        exit(0);
    }
}

/*********************************************************************
*********************************************************************/
int setupServerAddress(struct sockaddr_in* serverAddress, char* argv[])
{
    int portNumber = atoi(argv[2]); // Get the port number, convert to an integer from a string
    struct hostent* serverHostInfo;

    memset((char*)serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	serverAddress->sin_family = AF_INET; // Create a network-capable socket
	serverAddress->sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname(argv[1]); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&(*serverAddress).sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

    return 0;
}

/*********************************************************************
*********************************************************************/
int createTCPSocket(int* socketFD)
{
    // Create the socket
    *socketFD = socket(AF_INET, SOCK_STREAM, 0);
    // If socket cannot be created, inform user and exit program.
	if (socketFD < 0)
    {
        error("CLIENT: ERROR opening socket");
    }
    return 0;
}

/*********************************************************************
*********************************************************************/
int connectToServer(int socketFD, struct sockaddr_in* serverAddress)
{
    // Connect to Server Address
    if (connect(socketFD, (struct sockaddr*)serverAddress, sizeof(*serverAddress)) < 0)
    {
        // If cannot connect to server, inform user and exit program.
        error("CLIENT: ERROR connecting");
    }
}

/*********************************************************************
*********************************************************************/
int getInput(char* input, int inputSize)
{
    if(fgets(input, inputSize, stdin))
    {
        char *p;
        // Replace Newline Character
        if(p = strchr(input, '\n'))
        {
            *p = '\0';
        }
        // Clear the Stream Buffer
        else
        {
            scanf("%*[^\n]");
            scanf("%*c");
            printf("Input Too Long. Input Cut\n");
        }
    }
    return 0;
}

/*********************************************************************
 * int sendMessage(char* source, char* message, int fileDescriptor)
 *  Sends a message to the connection.
 * Arguments:
 *  char* message - the message to send.
 *  int fileDescriptor - the file descriptor of the connection.
 * Returns:
 * 	0 on success.
*********************************************************************/
int sendMessage(char* message, int fileDescriptor)
{
	int charsWritten;

	charsWritten = send(fileDescriptor, message, strlen(message) + 1, 0); // Write to the server
	if (charsWritten < 0) { error("CLIENT: ERROR writing to socket"); }
	if (charsWritten < strlen(message)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	
	return 0;
}

/*********************************************************************
 * int recvMessage(char* message, int fileDescriptor)
 *  Recieves a message from a connection
 * Arguments:
 *  char* buffer - The location to hold the recieves message
 *  int fileDescriptor - the file descriptor of the connection.
 * Returns:
 * 	0 on success.
*********************************************************************/
int recvMessage(char* message, int fileDescriptor)
{
	int charsRead;

	memset(message, '\0', MAX_CHARACTERS); // Clear out the buffer again for reuse
	charsRead = recv(fileDescriptor, message, MAX_CHARACTERS - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) { error("CLIENT: ERROR reading from socket"); }

    // If Server Terminated Connection, Terminate Program.
    if (charsRead == 0)
    {
        // close(fileDescriptor);
        // exit(0);
        quit(fileDescriptor);
    }

	return 0;
}

/*********************************************************************
*********************************************************************/
int checkQuit(char* buffer, char* exitCommand, int socketFD)
{
    if (!strcmp(buffer, exitCommand))
    {
        // close(socketFD);                // Close the Socket
        // exit(0);                        // Exit the Program
        quit(socketFD);
    }
    return 0;
}

/*********************************************************************
*********************************************************************/
void quit(int socketFD)
{
    close(socketFD);                // Close the Socket
    exit(0);                        // Exit the Program
}