
#include <stdio.h>
#include "debug.h"

#include "uthash.h"

struct etdc_table {
  unsigned int symbol; //original symbol
  int freq;

  unsigned char code[4]; //coded symbol
  int size; //size of the code

  UT_hash_handle hh; //hashable structure with uthash
};


void new_symbol( struct etdc_table **table, unsigned int s) {
  struct etdc_table *e;
  
  e = (struct etdc_table *)malloc(sizeof(struct etdc_table));
  e->symbol = s;
  e->freq = 1;

  INFO("Adding new symbol to etdc table");
  HASH_ADD_INT(*table, symbol, e);
}

void add_symbol(struct etdc_table **table, unsigned int s) {
  struct etdc_table *e;

  HASH_FIND_INT( *table, &s, e);

  if (NULL == e) {
    //add symbol to table
    INFO("No symbol found in table... creating one");
    new_symbol(table, s);
  } else {
    //increment frequency
    INFO("Incrementing frequency of symbol in etdc table");
    e->freq++;
  }
}

//void delete_symbol(struct etdc_table **table, struct etdc_table *symbol) {
//  //delete symbol in table
//}

void delete_table(struct etdc_table **table) {
  //delete all hash table
  struct etdc_table *curr, *tmp;

  HASH_ITER(hh, *table, curr, tmp) {
    HASH_DEL(*table, curr);  /* delete item from hash table */
    free(curr);            /* optional- if you want to free  */
  }

}


int freq_sort(struct etdc_table *a, struct etdc_table *b) {
  return (b->freq - a->freq);
}

void sort_table(struct etdc_table **table) {
  //by frequency
  HASH_SORT(*table, freq_sort);
}

void print_table(struct etdc_table *table) {
    struct etdc_table *s;
    int i, j;

    INFO("Printing table");
    for(s=table, j=0; s != NULL; s=s->hh.next, j++) {
        printf("symbol %u: freq %d\n", s->symbol, s->freq);

	printf("rank: %u code: ", j);
	for ( i = 0; i < s->size; i++) {
	  printf("%u ", s->code[i]); 
	}
	printf("\n");
    }
}



void gen_codes_v1(struct etdc_table *table) {
  struct etdc_table *s;
  int i, x;
  for(s=table, i=0; s != NULL; s=s->hh.next, i++) {
    //printf("i: %d\n", i);
    if ( i < 128 ) {
      s->code[0] = 128 + i % 128;
      s->size = 1;
    }
    else if ( 128 <= i && i < 16512 ) {
      s->code[1] = 128 + i % 128;
      x = i/128 - 1;
      s->code[0] = x % 128;
      s->size = 2;
    }
    else if ( 16512 <= i && i < 2113664 ) {
      s->code[2] = 128 + i % 128;
      x = i/128 - 1;
      s->code[1] = x % 128;
     
      x = x/128 - 1;
      s->code[0] = x % 128;
      
      s->size = 3;
    }
    else if ( 2113664 <= i && i < 270549120 ) {
      s->code[3] = 128 + i % 128;
      x = i/128 - 1;
      
      s->code[2] = x % 128;
      x = x/128 - 1;
      
      s->code[1] = x % 128;
      x = x/128 - 1;
      
      s->code[0] = x % 128;
      s->size = 4;
    }
  }
}

void gen_codes_v0(struct etdc_table *table) {
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

void generate_codes(struct etdc_table *table) {
  #ifdef PAPER
  printf("Using paper algorithm\n");
  gen_codes_v0(table);
  #else
  printf("Using diego algorithm\n");
  gen_codes_v1(table);
  #endif
}


int etdc_1stpass(struct etdc_table **table, unsigned int *input, int size) {
  int i;

  //adding symbols to hash table
  for(i = 0; i < size; i++) {
    add_symbol(table, input[i]);
  }

  //sorting by frequency (high values first)
  sort_table(table);

  //generate codes
  generate_codes(*table);

  return HASH_COUNT(*table);
}

int etdc_2ndpass(struct etdc_table **table, unsigned int *input, int size, unsigned char *output) {
  int i,j,ts;
  struct etdc_table *e;

  j = 0;
  for (i = 0; i < size; i++) {
    HASH_FIND_INT( *table, &input[i], e);
    
    ts = e->size;
    memcpy(&output[j], &(e->code), ts);

    /*
    int t;
    printf("%u ->", input[i]);
    for(t=0; t<ts; t++) printf(" %u", output[j+t]);
    printf("\n");
    */

    j += ts;
  }
  return j;
}

void readuint(unsigned int **items, int *size) {
  int n;
  int i;
  unsigned int *p;

  scanf("%d", &n);

  *size = n;

  p = malloc(sizeof(unsigned int)*n);

  for (i = 0; i < n; i++) {
    scanf("%u", &p[i]);
  }

  *items = p;
}


int main(int argc, char *argv[])
{
  struct etdc_table *table = NULL;
  /*
  unsigned int items[] = {1000,1000,1000,1000,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,
      27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,
      51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,
      75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,
      100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,
      119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,
      138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,
      157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,
      175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,
      193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,
      211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,
      229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,
      247,248,249,250,251,252,253,254,255,256,257,258,259,260,261,262,263,264,
      265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,281,282,
			  283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,
      27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,
      51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,
      75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,
      100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,
      119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,
      138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,
      157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,
      175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,
      193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,
      211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,
      229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,
      247,248,249,250,251,252,253,254,255,256,257,258,259,260,261,262,263,264,
      265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,281,282,
			283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300};
  int size = 602;
 
  */ 
  unsigned int *items;
  unsigned char *output;
  int size;
  int newsize;

  readuint(&items, &size);

  output = malloc(sizeof(unsigned int)*size);

  etdc_1stpass(&table, items, size);
  //print_table(table);

  newsize = etdc_2ndpass(&table, items, size, output);
 
  printf("oldsize = %lu\nnewsize = %u\n", size*sizeof(unsigned int), newsize);
  printf("Deleting table\n");
  delete_table(&table);
  print_table(table);
  
  printf("END\n");
  return 0;
}
