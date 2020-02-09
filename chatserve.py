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
    Code Adapted From:
        Python 3 Socket Documentation
            > https://docs.python.org/3/library/socket.html
    Course Name:     CS 372 Intro to Computer Networks
    Last Modified:   02/08/2020
'''

from sys import argv
from socket import socket, AF_INET, SOCK_STREAM
from signal import signal, SIGINT

def getPort(argv, default):
    '''
    Returns the selected port from the command line argument.
    Pre-Conditions:
        argv = The array of arguments from the command line.
        default = The default port to return.
    Post-Conditions:
        Returns port number
    '''
    # If there are arguments on the command line, return the argument
    if len(argv) > 1:
        return int(argv[1])
    return default

def initServer(hostName, portNumber):
    '''
    Initializes a TCP Listen Server.
    Adapted from https://docs.python.org/3/library/socket.html
    Pre-Conditions:
        hostName = The server's host
        portNumber = The port the server is running on
    Post-Conditions:
        returns the server socket
    '''
    serverSocket = socket(AF_INET, SOCK_STREAM) # Create the Socket
    serverSocket.bind((hostName, portNumber))   # Enable Socket to Begin Listening
    serverSocket.listen(1)                      # Flip the Socket On

    # Show that the Server is Running
    print('Server Running on Port', portNumber, 'and Ready to Receive. Press Ctrl + C to End.')

    return serverSocket

def recvMessage(connectionSocket, maxCharacters, encoding):
    '''
    Receives and Decodes a message received from the client
    Pre-Conditions:
        connectionSocket = The socket that holds the connection
        maxCharacters = The maximum characters to receive
        encoding = The type of encoding
    Post-Conditions:
        Returns the decoded message from the client
    '''
    return connectionSocket.recv(maxCharacters).decode(encoding)[:-1]

def prepareMessage(message, hostName, separator):
    '''
    Formats the message to send to the client
    Pre-Conditions:
        message = The message the user has inputted
        hostName = The name of the host
        separator = The characters separating the host name from the message
    Post-Conditions:
        Returns the formatted message
    '''
    return hostName + separator + message

def sendMessage(message, connectionSocket, encoding):
    '''
    Encodes and Sends the message to the client
    Pre-Conditions:
        message = The message to send to the client
        connectionSocket = The socket of the client
        encoding = The type of encoding
    Post-Conditions:
        Message is sent to the client
    '''
    connectionSocket.send(message.encode(encoding))

def getInput(message, maxLength):
    '''
    Gets and returns the user's input
    Pre-Conditions:
        message = The displayed message before taking the user's input.
        maxLength = The maximum number of characters that can be sent to client
    Post-Conditions:
        Returns the user's input
    '''
    # Get Input
    userInput = input(message)
    # If userInput is too high, inform user and return the trinned message
    if len(userInput) > maxLength:
        print('ERROR: Too Many Characters. Part of Message Lost')
        return userInput[0:maxLength]
    # Otherwise return the user's normal input
    return userInput

def checkQuit(message, exitCommand):
    '''
    Checks to see if a exit command was received.
    Pre-Conditions:
        message = The message to check
        exitCommand = The command to terminate the connections.
    Post-Conditions:
        Returns True if message and exitCommand match, otherwise False.
    '''
    if (message == exitCommand):    # Exit Command Received
        return True
    return False                    # Exit Command Not Received

def terminateConnection(addr, conncetionSocket, terminator):
    '''
    Notifies the connection has been ended and terminates the connection to the client.
    Pre-Conditions:
        addr = The address
        connectionSocket = The socket to the client
        terminator = The client or the server that terminated the connection
    Post-Conditions:
        Connection socket to client is ended
    '''
    # Close the connection socket to the client
    connectionSocket.close()
    # Show that the connection has been terminated.
    print('Connection', addr, 'Terminated by', terminator + ".")

def signalHandler(signal, frame):
    '''
    The function that will run once the SIGINT command is received.
    Adapted from https://stackoverflow.com/questions/1112343/how-do-i-capture-sigint-in-python
    Pre-Conditions:
        signal = The signal number.
        frame = The current stack frame.
    Post-Conditions:
        Print that SIGINT was recieved and terminate the program.

    '''
    # Show that the program is ending
    print('\nSIGINT detected. Terminating Program...')
    # Exit the Program
    exit(0)

if __name__ == "__main__":
    MAX_CHARACTERS = 500        # The Maximum of Characters to Send/Receive
    ENCODING = 'utf-8'          # The Type of Encoding
    HOST_NAME = 'HOST'          # The Name to Display to Clients
    SEPARATOR = '> '            # The Symbols that Separate the Host Name from the Message
    QUIT_CMD = '\\quit'         # The Command to Terminate the Connection
    HOST = 'localhost'          # The Host of the Server
    PORT = getPort(argv, 9999)  # The Port the Server is Using

    signal(SIGINT, signalHandler)   # Set Signal Handler

    serverSocket = initServer(HOST, PORT)   # Initialize the TCP Listen Server

    # Continually Look for New Clients to Accept and Connect To
    while True:
        connectionSocket, addr = serverSocket.accept()

        # Once Connected, Initialize Communication with Client
        with connectionSocket:
            # Show the New Connection that has been established
            print('Connected by', addr)

            # Get/Send Messages from/to Client
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
                message = getInput(HOST_NAME + SEPARATOR, MAX_CHARACTERS - len(HOST_NAME) - len(SEPARATOR))

                # Process User's Input
                if checkQuit(message, QUIT_CMD):    # Terminate Connection if User Entered Quit Command
                    terminateConnection(addr, connectionSocket, "Server")
                    break
                else:                               # Prepare and Send Message to Client
                    message = prepareMessage(message, HOST_NAME, SEPARATOR)
                    sendMessage(message, connectionSocket, ENCODING)