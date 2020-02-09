# CS 372: Operating Systems / Programming Assignment #1
    This is a Client-Server Network Application using the Sockets API and TCP Protocol.
    The client is implemented using C.
    The server is implemented using Python 3.

# Compilation (Linux)
    Compile the client using the Makefile.
    To use the makefile, on terminal run the command "make".

# Execution (Linux)
    The Server:
    (1) On the terminal run the command "python3 chatserve.py [PORT NUMBER]".
    The Client:
    (1) After compiling, run the command "./chatclient [SERVER HOST] [PORT NUMBER]".

# Control
    The Server:
        Usage:
            Send messages when connection established to a client.
        Disconnecting:
            (a) Enter the command "\quit".
            (b) Connection will also terminate if client terminates the connection.
        Terminating the Program:
            Press "Ctrl + C".
    The Client:
        Usage:
            (1) Enter handle to show to the hosting server. (Max 10 characters.)
            (2) Send messages when connection is established.
        Disconnecting / Terminating the Program
            Entering the command "\quit" will disconnect the client from the server
            and terminate the program.
