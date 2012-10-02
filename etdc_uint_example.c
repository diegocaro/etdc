#include <stdio.h>
#include <stdlib.h>

#include "etdc.h"
#include "debug.h"
#include "timing.h"

void readraw(char *filename, unsigned int **output, int *size) {
  FILE *f;
  f = fopen(filename, "rb");
  
  fseek( f, 0, SEEK_END);
  *size = ftell(f)/sizeof(unsigned int)+1;
  printf("filesize: %d\n", *size * 4);
  printf("count items: %d\n", *size);
  *output = malloc(*size * sizeof(unsigned int));
  
  fseek( f, 0, SEEK_SET);
  
  fread(*output, sizeof(unsigned int), *size, f);
  
  fclose(f);
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
  for(s=table; s != NULL; s=s->hh.next) {
    fwrite(&s->symbol, sizeof(unsigned int), 1, f);
  }
  fclose(f);
}

int main(int argc, char *argv[])
{
  struct etdc_table *table = NULL;
  unsigned char *output;
  int voc_size;
  int newsize;



  unsigned int *items;
  int size;
  readraw(argv[1], &items, &size);
  
  output = malloc(sizeof(unsigned char)*size*4);

  voc_size = firstpass(&table, items, size);

  #ifndef BENCHMARK
  etdc_print(table);
  #endif

  startTimer();
  newsize = etdc_encode(&table, items, size, output);
  printf("encoding = %lf (%ld) \n", timeFromBegin(), realTimeFromBegin());

  writefile(argv[2], table, output, newsize, voc_size, size);
  
  printf("oldsize = %lu\nnewsize = %d + %lu = %lu\n", 
	 size*sizeof(unsigned int), newsize, 
	 sizeof(unsigned int)*voc_size,
	 newsize+sizeof(unsigned int)*voc_size);

  printf("Deleting table\n");
  etdc_free(&table);
  

  free(output);

  printf("END\n");
  return 0;
}
