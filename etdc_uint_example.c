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
  *output = (unsigned int*)malloc(*size * sizeof(unsigned int));
  
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
  printf("realsize writefile: %d\n",realsize);
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


void readvocfile(char *filename, unsigned int **table, unsigned int *voc_size) {
  char vocfile[255];
  sprintf(vocfile, "%s.voc", filename);
  FILE *f;

  f = fopen(vocfile, "rb");
  fread(voc_size, sizeof(unsigned int), 1, f);

  *table = (unsigned int*)malloc(sizeof(unsigned int)*(*voc_size));
  fread(*table, sizeof(unsigned int), *voc_size, f);

  fclose(f);
}

void decompress_file(char *filein, char *fileout, unsigned int *items, unsigned int itemsize) {
  unsigned int *table;
  unsigned int vocsize;

  unsigned int old_size;
  unsigned int real_size;

  unsigned char *compressed;
  unsigned int *raw;

  FILE *f;

  readvocfile(filein, &table, &vocsize);
  printf("vocsize: %d\n", vocsize);

  f = fopen(filein, "rb");
  fread(&real_size, sizeof(unsigned int), 1, f);
  fread(&old_size, sizeof(unsigned int), 1, f);
  compressed = (unsigned char *) malloc(sizeof(unsigned char)*old_size);
  fread(compressed, sizeof(unsigned char), old_size, f);
  fclose(f);


  raw = (unsigned int*) malloc(sizeof(unsigned int) * real_size);
  etdc_decode(table, vocsize,compressed,old_size,raw,real_size);

  //g = fopen(fileout, "wb");
  //fwrite(&raw, sizeof(unsigned int), real_size,g);
  //fclose(g);


  unsigned int i;
  for(i=0; i < vocsize; i++) {
    printf("rank %d symbol %u\n", i, table[i]);
  }

  printf("realsize: %d\nitemsize: %d\n",real_size, itemsize);
  printf("compressed size: %d\n",old_size);
  for( i = 0; i < real_size; i++) {
    if (raw[i] != items[i])
        printf("diff %d %u-%u\n", i, raw[i], items[i]);
  }

}

int main(int argc, char *argv[])
{
  struct etdc_table *table = NULL;
  unsigned char *output;
  int voc_size;
  int newsize;

  //unsigned int *voc;

  unsigned int *items;
  int size;
  readraw(argv[1], &items, &size);

  output = (unsigned char*)malloc(sizeof(unsigned char)*size*5);

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


  decompress_file(argv[2], NULL, items, size);


  printf("END\n");
  return 0;
}
