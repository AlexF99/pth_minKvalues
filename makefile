run:
	gcc -Wall -O3 -c chrono.c -o chrono.o
	gcc -Wall -O3 -c heap.c -o heap.o -lm
	gcc -Wall -O3 heap.o chrono.o main.c -o acharKMenores -lpthread -lm

compileSingle:
	gcc -Wall -O3 acharKMenores.c -o acharKMenores -lpthread -lm

turnin:
	tar -czvf aopf20_wps20.tar.gz *.c *.h makefile planilhaJ6.ods relatorio*.pdf *.sh

clean:
	rm acharKMenores
	rm *.o