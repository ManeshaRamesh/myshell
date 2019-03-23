output: main.o shell.o
	gcc shell.o main.o -o myshell
main.o: main.c
	gcc -c main.c
shell.o:shell.c shell.h
	gcc -c shell.c
clean:
	rm -f *.o myshell
	rm -f history.txt
