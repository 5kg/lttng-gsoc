all: dyntp.so mutator mutatee

dyntp.so: dyntp.o
	gcc -shared  -fPIC -DPIC $^ -ldl -llttng-ust -O2 -o $@

dyntp.o: dyntp.c ust_test.h
	gcc -I. -fno-strict-aliasing -Wall -g -O2 -c $< -fPIC -DPIC -o $@

mutator: mutator.o
	g++ -g $^ -L/usr/lib/dyninst -ldyninstAPI -o $@

mutator.o: mutator.cpp
	g++ -g $< -c -o $@

mutatee: mutatee.o
	gcc $^ -std=gnu99 -g -lrt -o $@

mutatee.o: mutatee.c
	gcc $< -std=gnu99 -g -c -o $@

.PHONY: clean
clean:
	rm -f *.o *.so mutator mutatee
