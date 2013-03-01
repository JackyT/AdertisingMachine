all:
	gcc -Wall main.c -o server 
clean:
	rm -f *~
	rm -f *.o 
	rm -f *.swp
	rm -f server
