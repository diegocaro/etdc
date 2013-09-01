#include <stdio.h>
#include <stdlib.h>

#include "etdc.h"
#include "debug.h"
#include "timing.h"

void readuint(unsigned int **items, int *size) {
  int n;
  int i;
  unsigned int *p;

  scanf("%d", &n);

  *size = n;

  p = (unsigned int*)malloc(sizeof(unsigned int)*n);

  for (i = 0; i < n; i++) {
    scanf("%u", &p[i]);
  }

  *items = p;
}


int firstpass(struct etdc_table **table, unsigned int *input, int size) {
  int i;

  //adding symbols to hash table

  startTimer();
  for(i = 0; i < size; i++) {
    etdc_add(table, input[i]);
  }
  //printf("add hashtable time = %lf (%ld) \n", timeFromBegin(), realTimeFromBegin());

  //sorting by frequency (high values first)

  startTimer();
  etdc_sort(table);
  //printf("sorting hashtable time = %lf (%ld) \n", timeFromBegin(), realTimeFromBegin());

  //generate codes
  startTimer();
  etdc_gencodes(*table);
  printf("generating = %lf (%ld) \n", timeFromBegin(), realTimeFromBegin());

  return etdc_size(*table);
}

void writefile(char *filename, struct etdc_table *table, unsigned char *output, int newsize, int vocsize, int realsize) {
  FILE *f;
  struct etdc_table *s;

  char vocfile[255];
  sprintf(vocfile, "%s.voc", filename);

  f = fopen(filename, "wb");
  fwrite(&realsize, sizeof(unsigned int), 1, f);
  fwrite(&newsize, sizeof(unsigned int), 1, f);
  fwrite(output, sizeof(unsigned char), newsize, f);
  fclose(f);

  f = fopen(vocfile, "wb");
  fwrite(&vocsize, sizeof(unsigned int), 1, f);
  for(s=(struct etdc_table*)table; s != (struct etdc_table*)NULL; s=(struct etdc_table*)s->hh.next) {
    fwrite(&s->symbol, sizeof(unsigned int), 1, f);
  }
  fclose(f);
}

int main(int argc, char *argv[])
{
  struct etdc_table *table = NULL;
  unsigned char *output;
  unsigned int voc_size;
  int newsize;
  unsigned int *voctable;

  unsigned int *items;
  unsigned int size;
  unsigned int i;

  unsigned int *decoded;

  size = 10000000;
  items = (unsigned int *)malloc(sizeof(unsigned int)*size);
  for(i=0; i < size; i++) items[i]=i;

  
  output = (unsigned char*)malloc(sizeof(unsigned int)*size);
  voc_size = firstpass(&table, items, size);
  printf("Table: "); etdc_print(table);


  newsize = etdc_encode(&table, items, size, output);

  voctable = (unsigned int*) malloc(sizeof(unsigned int)*voc_size);
  etdc_voc2uint(table,voctable);
  for (i=0; i<voc_size;i++) printf("voc[%u] = %u\n", i, voctable[i]);
  
  etdc_free(&table);

  decoded = (unsigned int*)malloc(sizeof(unsigned int)*size);
  etdc_decode(voctable, voc_size, output, newsize, decoded, size);

  for(i=0; i<size; i++) {
    if(items[i] != decoded[i]) {
      printf("waaa %u %u\n", items[i], decoded[i]);
    }

  }

  free(output);

  printf("END\n");
  return 0;
}
