CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -pthread

# my project details
EXEC = amazing
OBJS = src/AMStartup.o src/AMClient.o src/amazing.o 
SRCS = src/AMStartup.c src/AMClient.c src/amazing.c

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

src/AMStartup.o: src/AMStartup.c src/AMClient.h
	$(CC) $(CFLAGS) -o src/AMStartup.o -c src/AMStartup.c

src/AMClient.o: src/AMClient.c src/AMClient.h
	$(CC) $(CFLAGS) -o src/AMClient.o -c src/AMClient.c

src/amazing.o: src/amazing.c src/amazing.h
	$(CC) $(CFLAGS) -o src/amazing.o -c src/amazing.c

debug: $(SRCS)
	$(CC) $(CFLAGS) -g -c $(SRCS)
	$(CC) $(CFLAGS) -g -o $(EXEC) $(OBJS)

clean:
	rm -f *~
	rm -f *#
	rm -f amazing
	rm -f src/*.o

