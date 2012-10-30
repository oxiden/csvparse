CC = gcc
CFLAGS = -Wall -O2
BINFILE = csvparse
SRC = csvparse.c debugger.c command_option.c
csvparse:
	$(CC) $(CFLAGS) -o $(BINFILE) $(SRC)
debug:
	$(CC) $(CFLAGS) -DDEBUG -o $(BINFILE) $(SRC)
clean:
	if [ -e $(BINFILE) ]; then rm $(BINFILE); fi
