run:
	gcc -Wall -c heap.c -o heap.o -lm
	gcc -Wall heap.o main.c -o acharKMenores -lpthread -lm
