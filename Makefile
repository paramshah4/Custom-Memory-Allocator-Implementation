CC = cc
CFLAGS = -g

all: memgrind

memgrind:
	$(CC) $(CFLAGS) memgrind.c -o memgrind

clean:
	rm -rf memgrind *.o

