/*
  6PACK - file compressor using FastLZ (lightning-fast compression library)
  Copyright (C) 2007-2020 Ariya Hidayat <ariya.hidayat@gmail.com>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include "fastlz.h"
#include "fastlz_conf.h"
#include "checksum.h"

/* magic identifier for 6pack file */
static unsigned char const sixpack_magic[8] = {137, '6', 'P', 'K', 13, 10, 26, 10};


static void write_chunkheader(int id, int options, unsigned long size, unsigned long checksum, unsigned long extra,unsigned char* outbuffer) 
{
  outbuffer[0] = id & 255;
  outbuffer[1] = id >> 8;
  outbuffer[2] = options & 255;
  outbuffer[3] = options >> 8;
  outbuffer[4] = size & 255;
  outbuffer[5] = (size >> 8) & 255;
  outbuffer[6] = (size >> 16) & 255;
  outbuffer[7] = (size >> 24) & 255;
  outbuffer[8] = checksum & 255;
  outbuffer[9] = (checksum >> 8) & 255;
  outbuffer[10] = (checksum >> 16) & 255;
  outbuffer[11] = (checksum >> 24) & 255;
  outbuffer[12] = extra & 255;
  outbuffer[13] = (extra >> 8) & 255;
  outbuffer[14] = (extra >> 16) & 255;
  outbuffer[15] = (extra >> 24) & 255;
  return;
}

unsigned long LZ77_CompressDataInit(unsigned long datalen, unsigned char* outbuffer, unsigned long* outlen)
{
  int i=0;
  if(*outlen >= (sizeof(sixpack_magic) + BLOCKCHUNK_HEADERSIZE))
  {
    for(i=0;i<sizeof(sixpack_magic);i++)
    {
      outbuffer[i] = sixpack_magic[i];
    }
    write_chunkheader(PATTERN_ID_TOTALHEADER,PATTERN_NOLZ77,datalen,0x00,0x00,&outbuffer[sizeof(sixpack_magic)]);
    unsigned long checksum = 1;
    checksum = update_adler32(checksum, outbuffer, (sizeof(sixpack_magic) + 8));
    write_chunkheader(PATTERN_ID_TOTALHEADER,PATTERN_NOLZ77,datalen,checksum,0x00,&outbuffer[sizeof(sixpack_magic)]);
    return (sizeof(sixpack_magic) + BLOCKCHUNK_HEADERSIZE);
  }
  return 0;
}

unsigned long LZ77_CompressDataBlock(unsigned long bytes_read, const unsigned char* const inbuffer, int level, unsigned char* result, unsigned long resultlen)
{
  int compress_method = 1;
  unsigned long chunk_size = 0;
  unsigned long checksum  = 1;
  if(bytes_read > BLOCK_SIZE && resultlen != 2*BLOCK_SIZE)
  {
    return 0;
  }

  /* too small, don't bother to compress */
  if (bytes_read < 32) compress_method = 0;


  int i=0;
#ifdef DEBUG
  for(i=0;i<bytes_read;i++)
  {
    printf("0x%02X, ",inbuffer[i]);
  }
  printf("\n");
#endif
  /* write to output */
  switch (compress_method) {
    /* FastLZ */
    case 1:
      chunk_size = (unsigned long)fastlz_compress_level(level, inbuffer, bytes_read, &result[BLOCKCHUNK_HEADERSIZE]);
      checksum = update_adler32(1L, &result[BLOCKCHUNK_HEADERSIZE], chunk_size);
      write_chunkheader(PATTERN_ID_DATAHEADER, PATTERN_LZ77_LEVEL1, chunk_size, checksum, bytes_read,result);
      break;

    /* uncompressed, also fallback method */
    case 0:
    default:
      checksum = 1L;
      for(i=0;i<bytes_read;i++)
      {
        result[BLOCKCHUNK_HEADERSIZE+i] = inbuffer[i];
      }
      checksum = update_adler32(checksum, &result[BLOCKCHUNK_HEADERSIZE], bytes_read);
      write_chunkheader(PATTERN_ID_DATAHEADER, PATTERN_NOLZ77, bytes_read, checksum, bytes_read,result);
      break;
  }
  return chunk_size+BLOCKCHUNK_HEADERSIZE;
}




