all: demo

demo: dynamic.c
	gcc $^ -llttng-ust -llttng-ust-tracepoint -o $@

clean:
	rm -f demo core.*
