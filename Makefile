TP_NAME=tp0

build:
	gcc -pedantic -Wall -Werror $(TP_NAME).c -o $(TP_NAME)
clean:
	rm -f $(TP_NAME)
	rm -f *.o
