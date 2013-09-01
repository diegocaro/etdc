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

  p = (unsigned int *)malloc(sizeof(unsigned int)*n);

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
  int voc_size;
  int newsize;

  #ifndef BENCHMARK
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
      283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,
      0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,
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

  #else
  unsigned int *items;
  int size;
  readuint(&items, &size);
  #endif
  
  output = (unsigned char *)malloc(sizeof(unsigned int)*size);

  voc_size = firstpass(&table, items, size);
  #ifndef BENCHMARK
  etdc_print(table);
  #endif

  startTimer();
  newsize = etdc_encode(&table, items, size, output);
  printf("encoding = %lf (%ld) \n", timeFromBegin(), realTimeFromBegin());

  #ifdef BENCHMARK
  writefile(argv[1], table, output, newsize, voc_size, size);
  #endif
  
  printf("oldsize = %lu\nnewsize = %d + %lu = %lu\n", 
	 size*sizeof(unsigned int), newsize, 
	 sizeof(unsigned int)*voc_size,
	 newsize+sizeof(unsigned int)*voc_size);

  printf("Deleting table\n");
  etdc_free(&table);
  etdc_print(table);
  
  #ifndef BENCHMARK
  int i;
  for(i=0; i<newsize;i++) {
    printf("%d ", output[i]);
  }
  #endif

  free(output);

  printf("END\n");
  return 0;
}
