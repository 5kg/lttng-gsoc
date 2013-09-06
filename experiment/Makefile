all: demo

demo: dynamic.c
	gcc $^ -llttng-ust -o $@

clean:
	rm -f demo core.*
