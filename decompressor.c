#include <stdio.h>
#include <stdlib.h>
#include "timing.h"

#ifdef PAPER
int etdc_decode(unsigned int *table, unsigned int voc_size, unsigned char *input, unsigned int size, unsigned int *output, unsigned int realsize) {
  unsigned char *max;
  unsigned int c;

  int base;
  int tot;

  unsigned char *p;
  unsigned int *o;
  printf("decode paper\n");

  p = input;
  o = output;

  max = input + size;

  base = 0;
  tot = 128;
  c = 0;
  while(p < max) {
    //printf("reading 0x%02x\n", *p);

    if (*p < 128) {
      c = c*128 + *p;
      base = base + tot;
      tot = tot*128;
    }
    else {
      //printf("%d %d %d\n", c*128, *p, base);
      c = c*128 + base + (*p - 128) ;

      //printf("c: %u\n", c);
      if (c >= voc_size) {printf("error fatal, codigo excede tamaño vocabulario\n"); exit(1);}
      *(o++) = table[c];
      //printf("table[%u] = %u\n", c, table[c]);
      c=0;
      tot = 128;
      base = 0;
    }
    p++;
  }


  return realsize;
}

#elif defined(SLOW)

int etdc_decode(unsigned int *table, unsigned int voc_size, unsigned char *input, unsigned int size, unsigned int *output, unsigned int realsize) {
  unsigned char *max;
  unsigned int c, t;

  unsigned char *p;
  unsigned int *o;
  printf("decode faster \n");
  p = input;
  o = output;

  max = input + size;

  c = 0;
  while(p < max) {
    //printf("reading 0x%02x\n", *p);
    t = c;
    c = (c+1)*128 + *p;    
    //printf("c: %08x\nt: %08x\n", c, t);
    if (*p >= 128) {
      //printf("%d %d %d\n", c*128, *p, base);

      c = c - (t+1)*128 + t - 128;
      //c = c - 127*t - 256;

      //printf("c: %u\n", c);
      if (c >= voc_size) {printf("error fatal, codigo excede tamaño vocabulario\n"); exit(1);}
      *(o++) = table[c];
      //printf("table[%u] = %u\n", c, table[c]);
      c = 0;
    }
    p++;
  }


  return realsize;
}


#else

int etdc_decode(unsigned int *table, unsigned int voc_size, unsigned char *input, unsigned int size, unsigned int *output, unsigned int realsize) {
  unsigned char *max;
  unsigned int c;

  unsigned char *p;
  unsigned int *o;
  printf("decode diego \n");
  p = input;
  o = output;

  max = input + size;

  c = 0;
  while(p < max) {
    //printf("reading 0x%02x\n", *p);
    if ( *p < 128) {
      c = (c+1)*128 + *p;    
    }
    else {
      //printf("%d %d %d\n", c*128, *p, base);
      c = c + *p - 128 ;

      //printf("c: %u\n", c);
      if (c >= voc_size) {printf("error fatal, codigo excede tamaño vocabulario\n"); exit(1);}
      *(o++) = table[c];
      //printf("table[%u] = %u\n", c, table[c]);
      c = 0;
    }
    p++;
  }


  return realsize;
}
#endif 

unsigned int etdc_openfile(char *filename, unsigned int **table, unsigned int *voc_size, unsigned char **coded, unsigned int *size) {
  FILE *f;
  char vocfile[255];

  unsigned int realsize;
  unsigned int *t;
  unsigned char *buf;

  sprintf(vocfile, "%s.voc", filename);

  f = fopen(vocfile, "rb");
  fread(voc_size, sizeof(unsigned int), 1, f);
  
  t = malloc( sizeof(unsigned int)*(*voc_size));
  fread(t, sizeof(unsigned int), *voc_size, f);

  fclose(f);

  *table = t;

  f = fopen(filename, "rb");
  fread(&realsize, sizeof(unsigned int), 1, f);
  fread(size, sizeof(unsigned int), 1, f);
  buf = malloc( sizeof(unsigned char)*(*size));
  fread(buf, sizeof(unsigned char), *size, f);
  fclose(f);

  *coded = buf;

  return realsize;
}

int main( int argc, char *argv[]) {
  unsigned int *table;
  unsigned int size;

  unsigned int voc_size;
  unsigned char *coded;

  unsigned int *uncompressed;
  unsigned int realsize;

  realsize = etdc_openfile(argv[1], &table, &voc_size, &coded, &size);

  uncompressed = malloc( sizeof(unsigned int) * realsize);
  /*
  realsize = 1;
  size = 3;
  coded[0] = 0x00;
  coded[1] = 0x00;
  coded[2] = 0x80;
  */
  startTimer();
  etdc_decode(table, voc_size, coded, size, uncompressed, realsize);
  printf("decoding = %lf (%ld) \n", timeFromBegin(), realTimeFromBegin());

  free(table);
  free(coded);

  return 0;
}
