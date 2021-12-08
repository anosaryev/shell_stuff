all: shell.o
	gcc -o bish shell.o

shell.o: shell.c shell.h
	gcc -c shell.c

run:
	gcc -c shell.c
	gcc -o bish shell.o
	./bish

clean:
	rm -f bish
	rm -f *.o
