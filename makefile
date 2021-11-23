io.o : io.c
	gcc -c -Wall $<

tfs.o : tfs.c
	gcc -c -Wall $<

main.o : main.c
	gcc -c -Wall $<

tfs : io.o tfs.o main.o
	gcc $^ -o tfs

run : tfs
	./tfs

clean :
	rm -f *.o tfs
