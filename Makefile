CC = gcc
SRCS = chatclient.c

main:
	${CC} ${SRCS} -o chatclient

clean:
	rm -f chatclient