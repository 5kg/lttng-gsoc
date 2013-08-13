all: syringe demo

CFLAGS=-Wall

syringe: syringe.o main.o util.o
demo: demo.o

.PHONY: clean
clean:
	rm -f *.so *.o syringe demo
