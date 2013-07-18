all: libkaji.so demo

libkaji.so: kaji.o
	gcc -lpthread -shared $^ -o $@

kaji.o: kaji.c kaji.h
	gcc -fPIC -c $< -o $@

.PHONY: clean
clean:
	rm -f *.so *.o demo
