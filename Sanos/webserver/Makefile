webserver: webserver.o
	gcc -o webserver -lc -lsyllable webserver.o

webserver.o: webserver.c
	gcc -c -O2 -Wall -Wextra -Werror webserver.c

clean:
	rm -f webserver.o
	rm -f webserver


