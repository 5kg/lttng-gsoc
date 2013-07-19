all: libkaji.so demo

libkaji.so: kaji.o
	gcc -ldl -lpthread -llttng-ust -shared $^ -o $@

kaji.o: kaji.c kaji.h ust_kaji_test.h
	gcc -fPIC -I. -c $< -o $@

.PHONY: clean
clean:
	rm -f *.so *.o demo
