
CC=gcc
CFLAGS=-c -Wall -O9
LDFLAGS=-Wall -O9

debug: CFLAGS+=-g
debug: LDFLAGS+=-g
debug: clean all

all: etdc_example etdc_uint_example etdc_test

etdc_example: etdc.o
	$(CC) $(LDFLAGS) etdc.o etdc_example.c -o etdc_example

etdc_uint_example: etdc.o
	$(CC) $(LDFLAGS) etdc.o etdc_uint_example.c -o etdc_uint_example

etdc_test: etdc.o
	$(CC) $(LDFLAGS) etdc.o etdc_test.c -o etdc_test

etdc.o:
	$(CC) $(CFLAGS) etdc.c -o etdc.o 

clean:
	rm -f *.o etdc_test etdc_example etdc_uint_example
