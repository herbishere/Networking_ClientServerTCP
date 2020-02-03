from sys import argv
from socket import socket, AF_INET, SOCK_STREAM

MAX_CHARACTERS = 500
SERVER_PORT = 31392
HOST_NAME = "HOST>"
if len(argv) > 1:
    SERVER_PORT = argv[1]

if __name__ == "__main__":
    # Create TCP Socket
    serverSocket = socket(AF_INET, SOCK_STREAM)
    serverSocket.bind(('', SERVER_PORT))
    # Listen for TCP Requests
    serverSocket.listen(1)
    print('Server Running on Port', SERVER_PORT, 'and Ready to Receive.')
    while 1:
        # Get
        connectionSocket, addr = serverSocket.accept()
        while 1:
            # Get Message from Client
            clientMessage = connectionSocket.recv(MAX_CHARACTERS)
            print(clientMessage)
            # Get Message from Command Line
            serverMessage = input(HOST_NAME)
            # Send Message to Client if Quit Command not input
            if serverMessage != "\\quit":
                serverMessage = HOST_NAME + serverMessage
                connectionSocket.send(serverMessage)
            # Otherwise Close the Connection
            else:
                break
        connectionSocket.close()