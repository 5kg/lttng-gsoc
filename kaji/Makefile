all: libkaji.so demo

libkaji.so: kaji.o trampoline.o
	gcc -ldl -lpthread -llttng-ust -shared $^ -o $@

kaji.o: kaji.c kaji.h ust_kaji_test.h
	gcc -Wall -fPIC -I. -c $< -o $@

trampoline.o: trampoline.s
	gcc -fPIC -c $< -o $@

.PHONY: clean
clean:
	rm -f *.so *.o demo
