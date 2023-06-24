.PHONY: clean

RM = rm -f

webserv: webserv.o socklib.o
	gcc -o webserv webserv.o socklib.o

socklib.o: socklib.c socklib.h
	gcc -Wall -g -c socklib.c

webserv.o: webserv.c webserv.h socklib.h
	gcc -Wall -g -c webserv.c

clean:
	$(RM) *.o
