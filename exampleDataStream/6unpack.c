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

#ifdef DEBUG
#include <stdio.h>
#endif

/* magic identifier for 6pack file */
static unsigned char const sixpack_magic[8] = {137, '6', 'P', 'K', 13, 10, 26, 10};


static unsigned long readU16(const unsigned char* ptr) { return ptr[0] + (ptr[1] << 8); }

static unsigned long readU32(const unsigned char* ptr) {
  return ptr[0] + (ptr[1] << 8) + (ptr[2] << 16) + (ptr[3] << 24);
}

static void read_chunkheader(unsigned char* headerbuffer, int* id, int* options, unsigned long* size, unsigned long* checksum,
                       unsigned long* extra) 
{
  unsigned char buffer[BLOCKCHUNK_HEADERSIZE];
  int i=0;
  for(i=0; i<BLOCKCHUNK_HEADERSIZE; i++)
  {
    buffer[i] = headerbuffer[i];
  }

  *id = readU16(buffer) & 0xffff;
  *options = readU16(buffer + 2) & 0xffff;
  *size = readU32(buffer + 4) & 0xffffffff;
  *checksum = readU32(buffer + 8) & 0xffffffff;
  *extra = readU32(buffer + 12) & 0xffffffff;
}

unsigned long Decompress_DataBlock(unsigned char* inbuffer, int chunk_id, int chunk_options, unsigned long chunk_size, unsigned long chunk_checksum, unsigned long chunk_extra,unsigned char* result)
{
  unsigned long checksum = 0;
  if(PATTERN_ID_DATAHEADER == chunk_id)
  {
      switch (chunk_options) 
      {
        /* stored, simply copy to output */
        case 0:
          /* read one block at at time, write and update checksum */
          checksum = update_adler32(1, inbuffer, chunk_size);
          /* verify everything is written correctly */
          if (checksum != chunk_checksum) {
#ifdef DEBUG
            printf("\nError: checksum mismatch. Aborted.\n");
            printf("Got %08lX Expecting %08lX\n", checksum, chunk_checksum);
#endif
          }
          break;

        /* compressed using FastLZ */
        case 1:
          checksum = update_adler32(1L, inbuffer, chunk_size);
          /* verify that the chunk data is correct */
          if (checksum != chunk_checksum) {
#ifdef DEBUG
            printf("\nError: checksum mismatch. Skipped.\n");
            printf("Got %08lX Expecting %08lX\n", checksum, chunk_checksum);
#endif
          } else {
            /* decompress and verify */
            unsigned long decompresslen = fastlz_decompress(inbuffer, chunk_size, result, chunk_extra);
            if(decompresslen != chunk_extra)
            {
#ifdef DEBUG
              printf("Decompress Data Error, Decompress len: %ld, Expected len :%ld\n",decompresslen, chunk_extra);
#endif
              return 0;
            }
            else
            {
              return decompresslen;
            }
          }
          break;

        default:
#ifdef DEBUG
          printf("\nError: unknown compression method (%d)\n", chunk_options);
#endif
          break;
      }
  }
  return 0;
}

unsigned long Decompress_LZ77DataBlock(unsigned char* inbuffer, unsigned long inbufferlen , unsigned char* result, unsigned long maxout, unsigned long* compresslen)
{
  int chunk_id;
  int chunk_options;
  unsigned long chunk_size;
  unsigned long chunk_checksum;
  unsigned long chunk_extra;
  if(inbufferlen < BLOCKCHUNK_HEADERSIZE)
  {
#ifdef DEBUG
    printf("Decompress Wrong Data Header\n");
#endif
    return 0;
  }
  read_chunkheader(inbuffer,&chunk_id,&chunk_options,&chunk_size,&chunk_checksum,&chunk_extra);
  if(inbufferlen < chunk_size+BLOCKCHUNK_HEADERSIZE)
  {
#ifdef DEBUG
    printf("Decompress Wrong Data Header\n");
#endif
    return 0;
  }
  if(chunk_extra <= maxout)
  {
    unsigned long resultlen = Decompress_DataBlock(&inbuffer[BLOCKCHUNK_HEADERSIZE],chunk_id,chunk_options,chunk_size,chunk_checksum,chunk_extra,result);
    if(resultlen != chunk_extra)
    {
      return 0;
    }
    *compresslen = chunk_size+BLOCKCHUNK_HEADERSIZE;
    return resultlen;
  }
  else
  {
    return 0;
  }
  return 0;
}

int Decompress_DetectMagic(unsigned char* magic, unsigned long magiclen) {
  if(magiclen != sizeof(sixpack_magic))
  {
    return -1;
  }
  int c = 0;
  for (c = 0; c < 8; c++)
  {
    if (magic[c] != sixpack_magic[c]) 
      return -1;
  }
  return 0;
}

unsigned long Decompress_CheckTotalDataHeader(unsigned char* inbuffer, unsigned long inbufferlen, unsigned long* decompresslen, unsigned long* compresslen)
{
  int chunk_id;
  int chunk_options;
  unsigned long chunk_size;
  unsigned long chunk_checksum;
  unsigned long chunk_extra;
  if(BLOCKCHUNK_HEADERSIZE+sizeof(sixpack_magic) == inbufferlen)
  {
    if( Decompress_DetectMagic(inbuffer,sizeof(sixpack_magic)) == 0)
    {
      read_chunkheader(&inbuffer[sizeof(sixpack_magic)],&chunk_id,&chunk_options,&chunk_size,&chunk_checksum,&chunk_extra);
      if(PATTERN_ID_TOTALHEADER == chunk_id && 0x00 == chunk_options)  
      {
        unsigned long checksum = 1;
        checksum = update_adler32(checksum, inbuffer, (sizeof(sixpack_magic) + 8));
        if(checksum == chunk_checksum)
        {
          *compresslen = chunk_extra;
          *decompresslen = chunk_size;
          return sizeof(sixpack_magic) + BLOCKCHUNK_HEADERSIZE;
        }
        else
        {
          return 0;
        }
      }
      else
      {
        return 0;
      }
    } 
    else
    {
      return 0;
    }

  }
  else
  {
    return 0;
  }
  return 0;
}
