//
// You have to implement a the first pass function to create rank table.
//
// See etdc_example.c for an example of usage


#include <stdio.h>
#include "uthash.h"
#include "etdc.h"

int etdc_encode(struct etdc_table **table, unsigned int *input, int size, unsigned char *output) {
  int i,ts;
  struct etdc_table *e;
  int k;

  unsigned char *t;
  unsigned char *f;

  t = output;

  for (i = 0; i < size; i++) {
    HASH_FIND_INT( *table, &input[i], e);
    //printf("encoding i: %d -> %u -> ", i ,input[i]);
    ts = e->size;

    f = (unsigned char *) &e->code;

    for(k=0; k < ts; k++) {
      //printf("%u ", *f);
      *(t++) = *(f++);
    }
    //printf("\n");
    /*
    int t;
    printf("%u ->", input[i]);
    for(t=0; t<ts; t++) printf(" %u", output[j+t]);
    printf("\n");
    */

  }
  return (t - output);
}

int etdc_decode(unsigned int *table, unsigned int voc_size, unsigned char *input, unsigned int size, unsigned int *output, unsigned int realsize) {
  unsigned char *max;
  unsigned int c;

  unsigned char *p;
  unsigned int *o;

  p = input;
  o = output;

  max = input + size;

  c = 0;
  while(p < max) {
    //printf("reading 0x%02x %u\n", *p, *p);
    if ( *p < 128) {
      c = (c+*p+1)*128;
    }
    else {
      //printf("%d %d %d\n", c*128, *p, base);
      c = c + *p - 128 ;

      //printf("c: %u\n", c);
      //printf("table[%u] = %u\n", c, table[c]);
      if (c >= voc_size) {printf("error fatal, codigo excede tamaño vocabulario\n"); exit(1);}

      *(o++) = table[c];
      //printf("%d: table[%u] = %u\n", o-output-1, c, table[c]);
      c = 0;
    }
    p++;
  }


  return realsize;
}



void etdc_new_symbol( struct etdc_table **table, unsigned int s) {
  struct etdc_table *e;
  
  e = (struct etdc_table *)malloc(sizeof(struct etdc_table));
  e->symbol = s;
  e->freq = 1;

  //fprintf(stderr, "Adding new symbol to etdc table");
  HASH_ADD_INT(*table, symbol, e);
}

void etdc_add(struct etdc_table **table, unsigned int s) {
  struct etdc_table *e;

  HASH_FIND_INT( *table, &s, e);

  if (NULL == e) {
    //add symbol to table
    //fprintf(stderr,"No symbol found in table... creating one");
    etdc_new_symbol(table, s);
  } else {
    //increment frequency
    //fprintf(stderr,"Incrementing frequency of symbol in etdc table");
    e->freq++;
  }
}


int etdc_freq_sort(struct etdc_table *a, struct etdc_table *b) {
  return (b->freq - a->freq);
}

void etdc_sort(struct etdc_table **table) {
  //by frequency
  HASH_SORT(*table, etdc_freq_sort);
}

 
int etdc_size( struct etdc_table *table) {
  return HASH_COUNT(table);
}


void etdc_print(struct etdc_table *table) {
    struct etdc_table *s;
    int i, j;

    fprintf(stderr, "Printing table");
    for(s=table, j=0; s != NULL; s=s->hh.next, j++) {
        printf("symbol %u: freq %d\n", s->symbol, s->freq);

	printf("rank: %u code: ", j);
	for ( i = 0; i < s->size; i++) {
	  printf("%u ", s->code[i]); 
	}
	printf("\n");
    }
}


//void delete_symbol(struct etdc_table **table, struct etdc_table *symbol) {
//  //delete symbol in table
//}

void etdc_free(struct etdc_table **table) {
  //delete all hash table
  struct etdc_table *curr, *tmp;

  HASH_ITER(hh, *table, curr, tmp) {
    HASH_DEL(*table, curr);  /* delete item from hash table */
    free(curr);            /* optional- if you want to free  */
  }

  *table = NULL;
}


void etdc_gencodes(struct etdc_table *table) {
  struct etdc_table *s;
  int paux;
  int first;
  int num;
  int p;
  int i,k;
  first = 0;
  num = 128;
  k = 1;
  p = 0;
  for(s=table; s != NULL;) {
    paux = 0;

    for(; (s != NULL) && (paux < num); s=s->hh.next) {
      s->code[k - 1] = 128 + paux % 128;
      paux = paux / 128;

      for( i = k -2; i >= 0; i--) {
	s->code[i] = paux % 128;
	paux = paux / 128;
      }
      s->size = k;

      p++;
      paux = p - first;
    }
    k++;
    first = first + num;
    num = num*128;
  }

}


void etdc_voc2uint(struct etdc_table *table, unsigned int *voc) {
  struct etdc_table *s;
  for(s=table; s != NULL; s=s->hh.next) {
    *(voc++) = s->symbol;
  }
}
