#CC=g++
CFLAGS=-c -Wall -O3
CFLAGSDEBUG=-c -g -Wall -O0
LDFLAGS=-Wall -O3

all: etdc_example etdc_uint_example etdc_test

debug: CFLAGS=$(CFLAGSDEBUG)
debug: clean all



.c.o:
	$(CXX) $(CFLAGS) $< -o $@

etdc_example: etdc.o
	$(CXX) $(LDFLAGS) etdc.o etdc_example.c -o etdc_example

etdc_uint_example: etdc.o
	$(CXX) $(LDFLAGS) etdc.o etdc_uint_example.c -o etdc_uint_example

etdc_test: etdc.o
	$(CXX) $(LDFLAGS) etdc.o etdc_test.c -o etdc_test

clean:
	rm -f *.o etdc_test etdc_example etdc_uint_example
