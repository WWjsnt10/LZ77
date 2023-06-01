#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIXPACK_VERSION_MAJOR 0
#define SIXPACK_VERSION_MINOR 1
#define SIXPACK_VERSION_REVISION 0
#define SIXPACK_VERSION_STRING "snapshot 20070615"

#define MAX_INPUFILE_SIZE   (4*1024*1024)

static unsigned char HandleDataBuffer[MAX_INPUFILE_SIZE];
static unsigned char CompressDataBuffer[MAX_INPUFILE_SIZE];

#include "fastlz_conf.h"
#include "fastlz.h"
#include "6pack.h"


/********************************************************************************************************************************************/


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




unsigned long CompressTotalBuffer(unsigned char* inbuffer, unsigned long inbufferlen, unsigned char* outbuffer, unsigned long* outbufferlen,int level)
{
#ifdef DEBUG
    printf("inbufferlen:%ld\n",inbufferlen);
#endif
    unsigned long totaloutdatabufferlen = *outbufferlen;
    unsigned char headerbuffer[24];
    unsigned long headerbufferlen = 24;
    unsigned long retlen = LZ77_CompressDataInit(inbufferlen,headerbuffer,&headerbufferlen);
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
  printf("inpufilelen :%ld\n",infilebytes);
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
