run:
	gcc -Wall -O3 -c chrono.c -o chrono.o
	gcc -Wall -O3 -c heap.c -o heap.o -lm
	gcc -Wall -O3 heap.o chrono.o main.c -o acharKMenores -lpthread -lm

clean:
	rm *.o
	rm a.out
	rm acharKMenores
	resultado*