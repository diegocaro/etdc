
CC=gcc
CFLAGS=-c -Wall -O9
LDFLAGS=-Wall -O9

all: etdc_example etdc_uint_example

etdc_example: etdc.o
	$(CC) $(LDFLAGS) etdc.o etdc_example.c -o etdc_example

etdc_uint_example: etdc.o
	$(CC) $(LDFLAGS) etdc.o etdc_uint_example.c -o etdc_uint_example

etdc.o:
	$(CC) $(CFLAGS) etdc.c -o etdc.o 

clean:
	rm *.o etdc_example etdc_uint_example