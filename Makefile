CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb

# my project details
EXEC = query
OBJS = src/file.o src/web.o src/hashtable.o src/list.o src/query.o
SRCS = src/query.c src/hashtable.c src/list.c src/web.c src/file.c src/hashtable.h src/list.h src/web.h src/file.h

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

src/query.o: src/query.c src/list.h src/hashtable.h
	$(CC) $(CFLAGS) -o src/query.o -c src/query.c

src/list.o: src/list.c src/list.h
	$(CC) $(CFLAGS) -o src/list.o -c src/list.c

src/hashtable.o: src/hashtable.c src/hashtable.h
	$(CC) $(CFLAGS) -o src/hashtable.o -c src/hashtable.c

src/web.o: src/.c src/web.h
	$(CC) $(CFLAGS) -o src/web.o -c src/web.c

debug: $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS)

clean:
	rm -f *~
	rm -f *#
	rm -f amazing
	rm -f src/*.o

