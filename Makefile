CC = gcc
CFLAGS = -Wall -O2
BINFILE = csvparse
SRC = csvparse.h csvparse.c debugger.c debugger.h
csvparse:
	$(CC) $(CFLAGS) -o $(BINFILE) $(SRC)
debug:
	$(CC) $(CFLAGS) -DDEBUG -o $(BINFILE) $(SRC)
clean:
	if [ -e $(BINFILE) ]; then rm $(BINFILE); fi
