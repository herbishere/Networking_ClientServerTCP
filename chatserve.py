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
    Last Modified:   02/07/2020
'''

from sys import argv
from socket import socket, AF_INET, SOCK_STREAM
from signal import signal, SIGINT

def getPort(argv, default):
    '''
    '''
    if len(argv) > 1:
        return int(argv[1])
    return default

def initServer(hostName, portNumber):
    '''
    '''
    serverSocket = socket(AF_INET, SOCK_STREAM)
    serverSocket.bind((hostName, portNumber))
    serverSocket.listen(1)
    print('Server Running on Port', portNumber, 'and Ready to Receive. Press Ctrl + C to End.')
    return serverSocket

def recvMessage(connectionSocket, maxCharacters, encoding):
    '''
    '''
    return connectionSocket.recv(maxCharacters).decode(encoding)[:-1]

def prepareMessage(message, hostName, separator):
    return hostName + separator + message

def sendMessage(message, connectionSocket, encoding):
    '''
    '''
    connectionSocket.send(message.encode(encoding))

def getInput(message):
    '''
    '''
    return input(message)

def checkQuit(message, exitCommand):
    '''
    '''
    if (message == exitCommand):
        return True
    return False

def terminateConnection(addr, conncetionSocket, terminator):
    '''
    '''
    print('Connection', addr, 'Terminated by', terminator + ".")
    connectionSocket.close()

def signalHandler(signal, frame):
    '''
    https://stackoverflow.com/questions/1112343/how-do-i-capture-sigint-in-python
    '''
    print('\nSIGINT detected. Terminating Program...')
    exit(0)

if __name__ == "__main__":
    MAX_CHARACTERS = 500
    ENCODING = 'utf-8'
    HOST_NAME = 'HOST'
    SEPARATOR = '> '
    QUIT_CMD = '\\quit'
    HOST = 'localhost'
    PORT = getPort(argv, 9999)

    signal(SIGINT, signalHandler)   # Set Signal Handler

    serverSocket = initServer(HOST, PORT)

    while True:
        connectionSocket, addr = serverSocket.accept()

        with connectionSocket:
            print('Connected by', addr)
            while True:
                # Get Message From Client
                message = recvMessage(connectionSocket, MAX_CHARACTERS + 1,ENCODING)

                # Process Recieved Message
                if message:                         # Display Received Message, 
                    print(message)
                else:                               # Exit Loop if Client Terminated Connection
                    terminateConnection(addr, connectionSocket, "Client")
                    break

                # Get Message From User
                message = getInput(HOST_NAME + SEPARATOR)

                # Process User's Input
                if checkQuit(message, QUIT_CMD):    # Terminate Connection if User Entered Quit Command
                    terminateConnection(addr, connectionSocket, "Server")
                    break
                else:                               # Prepare and Send Message to Client
                    message = prepareMessage(message, HOST_NAME, SEPARATOR)
                    sendMessage(message, connectionSocket, ENCODING)