all: dyntp.so mutator bench

dyntp.so: dyntp.o
	gcc -shared  -fPIC -DPIC $^ -ldl -llttng-ust -o $@

dyntp.o: dyntp.c ust_test.h
	gcc -I. -fno-strict-aliasing -Wall -g -c $< -fPIC -DPIC -o $@

mutator: mutator.o
	g++ -g $^ -L/usr/lib/dyninst -Wl,-rpath=/usr/lib/dyninst -ldyninstAPI -o $@

mutator.o: mutator.cpp
	g++ -g $< -c -o $@

bench: bench.o
	gcc $^ -std=gnu99 -g -lrt -lpthread -o $@

bench.o: bench.c
	gcc $< -std=gnu99 -g -c -o $@

.PHONY: clean
clean:
	rm -f *.o *.so mutator bench
