customServer: customServer.o unp.o sqStack.o
	gcc -o customServer -ggdb -lpthread customServer.o unp.o sqStack.o

customServer.o: customServer.c
	gcc -c -ggdb -lpthread customServer.c
unp.o: unp.c
	gcc -c -ggdb unp.c
sqStack.o: sqStack.c
	gcc -c -ggdb sqStack.c

test: test.o
	gcc -o test test.o
test.o: test.c
	gcc -c test.c
clean:
	rm -rf *.o customServer
