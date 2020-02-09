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
 * Last Modified:   02/08/2020
**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define MAX_CHARACTERS 500      // Max Characters To Send to Server
#define MAX_HANDLE_SIZE 10      // Max Characters of Handle
#define NUM_ARGS 3              // Number of Arguments for Function
#define h_addr h_addr_list[0]   // The First Host Address

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
    int socketFD,       // File Descriptor for the Socket
        handleLength;   // Number of Characters of the Handle
    struct sockaddr_in serverAddress;   // The server address info
    char    buffer[MAX_CHARACTERS + 1], // The buffer for the keyboard input
            handle[MAX_HANDLE_SIZE + 1];// The user's handle

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
        getInput(buffer, MAX_CHARACTERS + 1 - handleLength);// Get User's Message
        checkQuit(buffer, "\\quit", socketFD);              // Quit if User sends Quit Command

        // Send Message to Server
        char message[MAX_CHARACTERS + 1];
        snprintf(message, MAX_CHARACTERS + 1, "%s> %s", handle, buffer);// Format Message
        sendMessage(message, socketFD);

        // Receive Message from Server
        recvMessage(buffer, socketFD);

        // Print Message
        printf("%s\n", buffer);
    }

    return 0;
}

/*********************************************************************
 * void error(const char *msg)
 *  Prints an error message and exits the pgoram
 * Pre-Conditions:
 *  msg = The error message
 * Post-Conditions:
 *  Error Message is Sent to Displayed to stderr
 *  Program terminates.
*********************************************************************/
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

/*********************************************************************
 * void checkArguments(int argc, char* functionName)
 *  Program ensures proper number of arguments for the function.
 *  Based off of a function used in Program 4 of CS344: Operating Systems
 * Pre-Conditions:
 *  int argc = The number of arguments
 *  char* functionName = The name of the called function
 * Post-Conditions:
 *  If there are too few arguments, informs user of usage and
 *  terminates the program.
*********************************************************************/
void checkArguments(int argc, char* functionName)
{
    if (argc < NUM_ARGS)
    {
        fprintf(stderr,"USAGE: %s serverhost port\n", functionName);
        exit(0);
    }
}

/*********************************************************************
 * int setupServerAddress(struct sockaddr_in* serverAddress, char* argv[])
 *  Initializes and Prepares the Server Address Struct that is used
 *  by the socket to connect to the server.
 *  Adapted from Program 4 of CS344: Operating Systems
 * Pre-Conditions:
 *  struct sockaddr_in* serverAddress = A pointer to the struct
 *      containing the server address information.
 *  char* argv[] = An array of the arguments that was input when the
 *      program was first initialized that contains the program name,
 *      the host name, and the port number.
 * Post-Conditions:
 *  Returns 0 if the server address was succesfully initialized to be
 *      used by the socket.
 *  Terminates if the client failed to find the host.
*********************************************************************/
int setupServerAddress(struct sockaddr_in* serverAddress, char* argv[])
{
    int portNumber = atoi(argv[2]); // Get the port number, convert to an integer from a string
    struct hostent* serverHostInfo;

    memset((char*)serverAddress, '\0', sizeof(serverAddress));  // Clear out the address struct
	serverAddress->sin_family = AF_INET;                        // Create a network-capable socket
	serverAddress->sin_port = htons(portNumber);                // Store the port number
	serverHostInfo = gethostbyname(argv[1]);                    // Convert the machine name into a special form of address

    // If server host was not found, terminate the program.
	if (serverHostInfo == NULL)
    {
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(0);
    }

    // Store in the address
	memcpy((char*)&(*serverAddress).sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);

    return 0;
}

/*********************************************************************
 * int createTCPSocket(int* socketFD)
 *  Initializes a TCP Socket.
 *  Adapted from Program 4 of CS344
 * Pre-Conditions:
 *  int* socketFD = A pointer to the file descriptor of the socket to
 *      be used.
 * Post-Conditions:
 *  Returns 0 when the socket is initialized and ready to connect to
 *      the server.
 *  Otherwise, prints an error and exits the program.
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
 * int connectToServer(int socketFD, struct sockaddr_in* serverAddress)
 *  Connects the Client to the Server.
 *  Adapted from Program 4 of CS344.
 * Pre-Conditions:
 *  int socketFD = The file descriptor of the socket.
 *  struct sockaddr_in* serverAddress = A pointer to the struct
 *      containing the server address information.
 * Post-Conditions:
 *  Returns 0 when the client is connected to the server.
 *  If it cannot connect to the server, prints an error and terminates
 *      the program.
*********************************************************************/
int connectToServer(int socketFD, struct sockaddr_in* serverAddress)
{
    // Connect to Server Address
    if (connect(socketFD, (struct sockaddr*)serverAddress, sizeof(*serverAddress)) < 0)
    {
        // If cannot connect to server, inform user and exit program.
        error("CLIENT: ERROR connecting");
    }

    return 0;
}

/*********************************************************************
 * int getInput(char* input, int inputSize)
 *  Retrieves and saves the user's keyboard input.
 *  Adapted from
 *      https://stackoverflow.com/questions/38767967/clear-input-buffer-after-fgets-in-c
 * Pre-Conditions:
 *  char* input = The location the input will be saved to.
 *  int inputSize = The maximum size of the string to save.
 * Post-Conditions:
 *  Returns 0 when input stores a string of max length inputSize.
 *  If the string exceeds inputSize, store only inputSize characters,
 *      inform the user, and drop the remaining characters.
*********************************************************************/
int getInput(char* input, int inputSize)
{
    if(fgets(input, inputSize, stdin))
    {
        char *pointer;  // Holds the location of the newline character

        // Replace Newline Character with an '\0' character
        if(pointer = strchr(input, '\n'))
        {
            *pointer = '\0';
        }
        // Clear the Input Buffer
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
 * int sendMessage(char* message, int fileDescriptor)
 *  Sends a message to the connected server.
 *  Adapted from Program 4 of CS344: Operating Systems
 * Pre-Conditions:
 *  char* message - the message to send.
 *  int fileDescriptor - the file descriptor of the connection.
 * Post-Conditions:
 * 	Returns 0 if message sent successfully.
 *  Returns 1 if message wasn't completely sent.
 *  Terminates the program and prints error if program couldn't write
 *      to the socket.
*********************************************************************/
int sendMessage(char* message, int fileDescriptor)
{
	int charsWritten;   // Number of characters sent to server.

	charsWritten = send(fileDescriptor, message, strlen(message) + 1, 0); // Write to the server
    
    // Terminate Program if message couldn't be sent.
	if (charsWritten < 0)
    {
        error("CLIENT: ERROR writing to socket");
    }

    // Inform user if whole message couldn't be sent.
	if (charsWritten < strlen(message))
    {
        printf("CLIENT: WARNING: Not all data written to socket!\n");
        return 1;
    }

	return 0;
}

/*********************************************************************
 * int recvMessage(char* message, int fileDescriptor)
 *  Recieves a message from the server. Exits program if connection
 *      was terminated by the server.
 *  Adapted from Program 4 of CS344: Operating Systems
 * Arguments:
 *  char* buffer = The location to hold the recieves message
 *  int fileDescriptor = the file descriptor of the connection.
 * Returns:
 * 	Returns 0 on successful message received.
 *  Terminates program if server has terminated the connection.
 *  Prints error message and terminates program if the socket couldn't
 *      be read from.
*********************************************************************/
int recvMessage(char* message, int fileDescriptor)
{
	int charsRead;  // Number of characters received from the server.

    // Clear out the buffer again for reuse
	memset(message, '\0', MAX_CHARACTERS);
    // Read data from the socket, leaving \0 at end
	charsRead = recv(fileDescriptor, message, MAX_CHARACTERS - 1, 0);

    // If socket could not be read from, terminate the program.
	if (charsRead < 0)
    {
        error("CLIENT: ERROR reading from socket");
    }

    // If Server Terminated Connection, Terminate Program.
    if (charsRead == 0)
    {
        quit(fileDescriptor);
    }

	return 0;
}

/*********************************************************************
 * int checkQuit(char* buffer, char* exitCommand, int socketFD)
 *  Checks to see if a quit command was sent.
 * Pre-Conditions:
 *  char* buffer = The buffer containing a message
 *  char* exitCommand = The string holding the sepcific exit command.
 *  int socketFD = The socket to close if the exit command was input.
 * Post-Conditions:
 *  Returns 0 if exit command was not found.
 *  Terminates program if exit command is found.
*********************************************************************/
int checkQuit(char* buffer, char* exitCommand, int socketFD)
{
    // If exit command is found, terminate the program.
    if (!strcmp(buffer, exitCommand))
    {
        quit(socketFD);
    }
    return 0;
}

/*********************************************************************
 * void quit(int socketFD)
 *  Closes the socket and terminates the program.
 * Pre-Conditions:
 *  int socketFD = The file descriptor to close.
 * Post-Conditions:
 *  Socket is closed and the program is terminated.
*********************************************************************/
void quit(int socketFD)
{
    close(socketFD);    // Close the Socket
    exit(0);            // Exit the Program
}