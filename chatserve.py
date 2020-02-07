'''
    Programmer Name: Herbert Diaz <diazh@oregonstate.edu>
    Program Name:    Program 1 - Chat Server
    Program Description:
        Usage: chatserve [portnum]
        This program serves as the server for a messaging system
        between a client and server using the TCP protocol. After
        connecting to and receiving a message from the user, the
        host can send messages until either they or the client
        terminates the connection with the '\\quit' (read: 
        backslash-quit) (b) command. To terminate the server, 
        send a SIGINT with ctrl + c in the terminal.
    Course Name:     CS 372 Intro to Computer Networks
    Last Modified:   02/05/2020
'''

from sys import argv
from socket import socket, AF_INET, SOCK_STREAM

ENCODING = "utf-8"
HOST_NAME = "HOST> "
EXIT_COMMAND = "\\quit"
MAX_CHARACTERS = 501
SERVER_PORT = 7777
if len(argv) > 1:
    SERVER_PORT = int(argv[1])

def initListenServer(portNumber):
    '''
    '''
    # Create TCP Socket
    serverSocket = socket(AF_INET, SOCK_STREAM)
    serverSocket.bind(('', portNumber))
    # Listen for TCP Requests
    serverSocket.listen(1)
    print('Server Running on Port', portNumber, 'and Ready to Receive.')

    return serverSocket

def recvMessage(connectionSocket, maxCharacters, encoding):
    '''
    '''
    return connectionSocket.recv(maxCharacters).decode(encoding)[:-1]

def sendMessage(message, connectionSocket, encoding):
    '''
    '''
    connectionSocket.send(message.encode(encoding))

def checkQuit(message, exitCommand):
    if (message == exitCommand):
        return True
    return False

def getInput(message):
    '''
    '''
    return input(message)

if __name__ == "__main__":

    serverSocket = initListenServer(SERVER_PORT)        # Create a TCP Socket and Listen for TCP Requests

    while 1:
        connectionSocket, addr = serverSocket.accept()  # Accept a Connection
        newConnection = True

        while 1:
            if newConnection:
                newConnection = False
                print("===New Connection Established===")
            quitFlag = False

            # Get Message from Client
            clientMessage = recvMessage(connectionSocket, MAX_CHARACTERS, ENCODING)

            # Check if Client Terminated Connection
            quitFlag = checkQuit(clientMessage, EXIT_COMMAND)

            if quitFlag:                                # End Connection if Exit Command Received.
                connectionSocket.close()
                break
            else:                                       # Otherwise, Print the Message
                print(clientMessage)

            serverMessage = getInput(HOST_NAME)         # Get Message from Command Line

            # Check if Server Terminated Connection
            quitFlag = checkQuit(serverMessage, EXIT_COMMAND)

            # If Not Quitting, Prepare Message to Send
            if not quitFlag:
                serverMessage = HOST_NAME + serverMessage

            # Send Message
            sendMessage(serverMessage, connectionSocket, ENCODING)

            # If Quitting, End Connection
            if quitFlag:
                connectionSocket.close()
                break