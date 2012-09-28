#ifndef __ETDC_H__
#define __ETDC_H__
#include "uthash.h"
struct etdc_table {
  unsigned int symbol; //original symbol
  int freq;

  unsigned char code[4]; //coded symbol

  int size; //size of the code

  UT_hash_handle hh; //hashable structure with uthash
};


int etdc_encode(struct etdc_table **table, unsigned int *input, int size, unsigned char *output);
int etdc_decode(unsigned int *table, unsigned int voc_size, unsigned char *input, unsigned int size, unsigned int *output, unsigned int realsize);

void etdc_new_symbol( struct etdc_table **table, unsigned int s);
void etdc_add(struct etdc_table **table, unsigned int s);

int etdc_freq_sort(struct etdc_table *a, struct etdc_table *b);
void etdc_sort(struct etdc_table **table);

int etdc_size( struct etdc_table *table);
void etdc_print(struct etdc_table *table);
void etdc_free(struct etdc_table **table);

void etdc_gencodes(struct etdc_table *table);

#endif
