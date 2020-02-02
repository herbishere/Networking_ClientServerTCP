from sys import argv

SERVER_HOST, SERVER_PORT = "localhost", 12000
if len(argv) > 1:
    SERVER_PORT = argv[1]

if __name__ == "__main__":
    print(SERVER_PORT)