all: libkaji.so demo client

CFLAGS=-Wall

libkaji.so: kaji.o trampoline.o util.o
	gcc -ldl -lpthread -llttng-ust -shared $^ -o $@

kaji.o: kaji.c kaji.h ust_kaji_test.h
	gcc -Wall -fPIC -I. -c $< -o $@

trampoline.o: trampoline.s
	gcc -fPIC -c $< -o $@

util.o: util.c util.h
	gcc -Wall -fPIC -I. -c $< -o $@

client: client.o util.o

.PHONY: clean
clean:
	rm -f *.so *.o demo client
