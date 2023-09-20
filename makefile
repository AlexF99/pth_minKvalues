run:
	gcc -Wall -c chrono.c -o chrono.o
	gcc -Wall -c heap.c -o heap.o -lm
	gcc -Wall heap.o chrono.o main.c -o acharKMenores -lpthread -lm
