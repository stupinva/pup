#ifndef __COMDEC__
#define __COMDEC__
#include "c_zlib.h"
#include "c_pack2.h"
#include "c_lzss.h"
#include "c_dat.h"

typedef struct comdec_s
{
  size_t(*dsize) (size_t ssize);
  bool_t(*compress) (unsigned char *s, size_t ssize, unsigned char *d,
                     size_t * dsize, int level);
  bool_t(*decompress) (unsigned char *s, size_t ssize, unsigned char *d,
                       size_t dsize);
} comdec_t;

static const comdec_t comdec_zlib = { &zlib_dsize, &zlib_compress, &zlib_decompress };
static const comdec_t comdec_pack2 = { &pack2_dsize, &pack2_compress, &pack2_decompress };
static const comdec_t comdec_lzss = { &lzss_dsize, &lzss_compress, &lzss_decompress };
static const comdec_t comdec_dat = { &dat_dsize, &dat_compress, &dat_decompress };

bool_t c_fadd(FILE * dst, const char *src_filename, size_t * size,
              size_t * compressed, const comdec_t * comdec, int level);
bool_t c_fextract(const char *dst_filename, FILE * src, size_t size,
                  size_t compressed, const comdec_t * comdec);

#endif
