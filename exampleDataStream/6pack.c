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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIXPACK_VERSION_MAJOR 0
#define SIXPACK_VERSION_MINOR 1
#define SIXPACK_VERSION_REVISION 0
#define SIXPACK_VERSION_STRING "snapshot 20070615"

#include "fastlz.h"

#undef PATH_SEPARATOR

#if defined(MSDOS) || defined(__MSDOS__) || defined(MSDOS)
#define PATH_SEPARATOR '\\'
#endif

#if defined(WIN32) || defined(__NT__) || defined(_WIN32) || defined(__WIN32__)
#define PATH_SEPARATOR '\\'
#endif

#ifndef PATH_SEPARATOR
#define PATH_SEPARATOR '/'
#endif

#undef SIXPACK_BENCHMARK_WIN32
#if defined(WIN32) || defined(__NT__) || defined(_WIN32) || defined(__WIN32__)
#if defined(_MSC_VER) || defined(__GNUC__)
#define SIXPACK_BENCHMARK_WIN32
#include <windows.h>
#endif
#endif

/* magic identifier for 6pack file */
static unsigned char sixpack_magic[8] = {137, '6', 'P', 'K', 13, 10, 26, 10};

#define MAX_INPUFILE_SIZE   (4*1024*1024)

static unsigned char HandleDataBuffer[MAX_INPUFILE_SIZE];
static unsigned char CompressDataBuffer[MAX_INPUFILE_SIZE];

#define BLOCK_SIZE                (1 * 1 * 1024)
#define BLOCKCHUNK_HEADERSIZE     (16)

#define PATTERN_ID_TOTALHEADER     1
#define PATTERN_ID_DATAHEADER      17

#define PATTERN_NOLZ77             0
#define PATTERN_LZ77_LEVEL1        1
#define PATTERN_LZ77_LEVEL2        2



/* prototypes */
static unsigned long update_adler32(unsigned long checksum, const void* buf, int len);
void usage(void);

/* for Adler-32 checksum algorithm, see RFC 1950 Section 8.2 */
#define ADLER32_BASE 65521
static unsigned long update_adler32(unsigned long checksum, const void* buf, int len) {
  const unsigned char* ptr = (const unsigned char*)buf;
  unsigned long s1 = checksum & 0xffff;
  unsigned long s2 = (checksum >> 16) & 0xffff;

  while (len > 0) {
    unsigned k = len < 5552 ? len : 5552;
    len -= k;

    while (k >= 8) {
      s1 += *ptr++;
      s2 += s1;
      s1 += *ptr++;
      s2 += s1;
      s1 += *ptr++;
      s2 += s1;
      s1 += *ptr++;
      s2 += s1;
      s1 += *ptr++;
      s2 += s1;
      s1 += *ptr++;
      s2 += s1;
      s1 += *ptr++;
      s2 += s1;
      s1 += *ptr++;
      s2 += s1;
      k -= 8;
    }

    while (k-- > 0) {
      s1 += *ptr++;
      s2 += s1;
    }
    s1 = s1 % ADLER32_BASE;
    s2 = s2 % ADLER32_BASE;
  }
  return (s2 << 16) + s1;
}

void usage(void) {
  printf("6pack: high-speed file compression tool\n");
  printf("Copyright (C) Ariya Hidayat\n");
  printf("\n");
  printf("Usage: 6pack [options]  input-file  output-file\n");
  printf("\n");
  printf("Options:\n");
  printf("  -1    compress faster\n");
  printf("  -2    compress better\n");
  printf("  -v    show program version\n");
#ifdef SIXPACK_BENCHMARK_WIN32
  printf("  -mem  check in-memory compression speed\n");
#endif
  printf("\n");
}

/**
 * Do it by Customer
*/
static unsigned long Get_InputFileDataStream(const char* input_file,unsigned char* inputbuffer, unsigned long  inbuffer_len)
{
  unsigned long filelen = 0;
  FILE* in = NULL;
  /* sanity check */
  in = fopen(input_file, "rb");
  if (!in) {
    printf("Error: could not open %s\n", input_file);
    return -1;
  }

  /* find size of the file */
  fseek(in, 0, SEEK_END);
  filelen = ftell(in);
  fseek(in, 0, SEEK_SET);

  if(filelen > inbuffer_len)
  {
      printf("inputbuffer is small: need %lu\n bytes\n",filelen);
      return 0;
  }
  size_t bytes_read = fread(inputbuffer,1,filelen,in);
  if(bytes_read != filelen)
  {
    fclose(in);
    printf("file is read error\n");
    return 0;
  }
  fclose(in);
  return filelen;
}

/**
 *  Customer need to modify the Function to get the all data
 * 
*/
static unsigned long Handle_InputFile(const char* input_file,unsigned char* filebuffer,  unsigned long  inbuffer_len)
{
  return Get_InputFileDataStream(input_file,filebuffer,inbuffer_len);
}


void write_chunkheader(int id, int options, unsigned long size, unsigned long checksum, unsigned long extra,unsigned char* outbuffer) 
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

unsigned long LZ77_Init(unsigned long datalen, unsigned char* outbuffer, unsigned long* outlen)
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

unsigned long CompressTotalBuffer(unsigned char* inbuffer, unsigned long inbufferlen, unsigned char* outbuffer, unsigned long* outbufferlen,int level)
{
#ifdef DEBUG
    printf("inbufferlen:%ld\n",inbufferlen);
#endif
    unsigned long totaloutdatabufferlen = *outbufferlen;
    unsigned char headerbuffer[24];
    unsigned long headerbufferlen = 24;
    unsigned long retlen = LZ77_Init(inbufferlen,headerbuffer,&headerbufferlen);
    int i=0;
    for(i=0; i<retlen; i++)
    {
      outbuffer[i] = headerbuffer[i];
    }

    unsigned long remaining = inbufferlen;
    unsigned long compressdatalen = retlen;

    while(remaining != 0)
    {
      if(compressdatalen + 2*BLOCK_SIZE > totaloutdatabufferlen)
      {
        printf("Compress Data Buffer OverFlow\n");
        return 0;
      }
      unsigned char* databuffer = &inbuffer[inbufferlen - remaining];
      unsigned char* result = &outbuffer[compressdatalen];
      if(remaining >= BLOCK_SIZE)
      {
        unsigned long compresssize = LZ77_CompressDataBlock(BLOCK_SIZE,databuffer,level,result,2*BLOCK_SIZE);
        remaining -= BLOCK_SIZE;
        compressdatalen += compresssize;
#ifdef DEBUG
        i = 0;
        printf("compresssize:%ld\n",compresssize);
        for(i = 0 ;i< compresssize;i++)
        {
          printf("0x%02X,", result[i]);
        }
        printf("\n");
#endif
      }
      else
      {
          /**
           * This is the Last Buffer Block need to be compressed
          */
        unsigned char* databuffer = &inbuffer[inbufferlen - remaining];
        unsigned char* result = &outbuffer[compressdatalen];
        unsigned long compresssize = LZ77_CompressDataBlock(remaining,databuffer,level,result,2*BLOCK_SIZE);
        remaining -= remaining;
        compressdatalen += compresssize;
#ifdef DEBUG
        i = 0;
        printf("compresssize:%ld\n",compresssize);
        for(i = 0 ;i< compresssize;i++)
        {
          printf("0x%02X,", result[i]);
        }
#endif
      }
      *outbufferlen = compressdatalen;
    }
    return remaining;
}




int main(int argc, char** argv) {
  int i;
  int compress_level;
  int benchmark;
  char* input_file;
  char* output_file;

  /* show help with no argument at all*/
  if (argc == 1) {
    usage();
    return 0;
  }

  /* default compression level, not the fastest */
  compress_level = 2;

  /* do benchmark only when explicitly specified */
  benchmark = 0;

  /* no file is specified */
  input_file = 0;
  output_file = 0;

  for (i = 1; i <= argc; i++) {
    char* argument = argv[i];

    if (!argument) continue;

    /* display help on usage */
    if (!strcmp(argument, "-h") || !strcmp(argument, "--help")) {
      usage();
      return 0;
    }

    /* check for version information */
    if (!strcmp(argument, "-v") || !strcmp(argument, "--version")) {
      printf("6pack: high-speed file compression tool\n");
      printf("Version %s (using FastLZ %s)\n", SIXPACK_VERSION_STRING, FASTLZ_VERSION_STRING);
      printf("Copyright (C) Ariya Hidayat\n");
      printf("\n");
      return 0;
    }

    /* test compression speed? */
    if (!strcmp(argument, "-mem")) {
      benchmark = 1;
      continue;
    }

    /* compression level */
    if (!strcmp(argument, "-1") || !strcmp(argument, "--fastest")) {
      compress_level = 1;
      continue;
    }
    if (!strcmp(argument, "-2")) {
      compress_level = 2;
      continue;
    }

    /* unknown option */
    if (argument[0] == '-') {
      printf("Error: unknown option %s\n\n", argument);
      printf("To get help on usage:\n");
      printf("  6pack --help\n\n");
      return -1;
    }

    /* first specified file is input */
    if (!input_file) {
      input_file = argument;
      continue;
    }

    /* next specified file is output */
    if (!output_file) {
      output_file = argument;
      continue;
    }

    /* files are already specified */
    printf("Error: unknown option %s\n\n", argument);
    printf("To get help on usage:\n");
    printf("  6pack --help\n\n");
    return -1;
  }

  if (!input_file) {
    printf("Error: input file is not specified.\n\n");
    printf("To get help on usage:\n");
    printf("  6pack --help\n\n");
    return -1;
  }

  if (!output_file && !benchmark) {
    printf("Error: output file is not specified.\n\n");
    printf("To get help on usage:\n");
    printf("  6pack --help\n\n");
    return -1;
  }

  unsigned long infilebytes = Handle_InputFile(input_file,HandleDataBuffer,MAX_INPUFILE_SIZE);
  for(i = 0;i<infilebytes;i++)
  {
      if(i%16 == 0)
      {
        printf("\n");
      }
      printf("0x%02X,",HandleDataBuffer[i]);
  }
  printf("\n\n");
  if(infilebytes != 0)
  {
    unsigned long compressbufferlen = MAX_INPUFILE_SIZE;
    unsigned long remaining = CompressTotalBuffer(HandleDataBuffer,infilebytes,CompressDataBuffer,&compressbufferlen,compress_level);
    if(remaining != 0)
    {
      printf("Compress Error\n");
    }
    else
    {
      for(i = 0;i<compressbufferlen;i++)
      {
          if(i%16 == 0)
          {
            printf("\n");
          }
          printf("0x%02X,",CompressDataBuffer[i]);
      }
    }
  }
  else
  {
    printf("handle inpufile error\n");
  }

  /*return pack_file(compress_level, input_file, output_file);*/

  /* unreachable */
  return 0;
}
