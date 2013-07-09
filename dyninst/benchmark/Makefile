all: dyntp.so mutator bench_notrace bench_trace

dyntp.so: dyntp.o
	gcc -shared  -fPIC -DPIC $^ -ldl -llttng-ust -o $@

dyntp.o: dyntp.c ust_tests_benchmark.h
	gcc -I. -fno-strict-aliasing -Wall -g -c $< -fPIC -DPIC -o $@

tp.o: tp.c ust_tests_benchmark.h
	gcc -I. -fno-strict-aliasing -Wall -g -c $< -o $@

mutator: mutator.o
	g++ -g $^ -L/usr/lib/dyninst -Wl,-rpath=/usr/lib/dyninst -ldyninstAPI -o $@

mutator.o: mutator.cpp
	g++ -g $< -c -o $@

bench_notrace: bench_notrace.o
	gcc $^ -std=gnu99 -g -O2 -lrt -lpthread -o $@

bench_notrace.o: bench.c
	gcc $< -std=gnu99 -g -O2 -c -o $@

bench_trace: bench_trace.o tp.o
	gcc $^ -std=gnu99 -g -O2 -lrt -ldl -llttng-ust -lpthread -o $@

bench_trace.o: bench.c
	gcc $< -std=gnu99 -g -O2 -DTRACING -c -o $@

.PHONY: clean
clean:
	rm -f *.o *.so mutator bench_notrace bench_trace
