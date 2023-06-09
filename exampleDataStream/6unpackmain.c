#define SIXPACK_VERSION_MAJOR 0
#define SIXPACK_VERSION_MINOR 1
#define SIXPACK_VERSION_REVISION 0
#define SIXPACK_VERSION_STRING "0.1.0"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fastlz.h"
#include "fastlz_conf.h"
#include "6unpack.h"

#define TOTALBLOCK_SIZE 65536


static unsigned char Decompress_TestResultBuffer[TOTALBLOCK_SIZE];



unsigned long Decompress_TotalDataBlock(unsigned char* inbuffer,unsigned long inbufferlen, unsigned char* outbuffer, unsigned long* outbufferlen)
{
    unsigned long decompresslen = 0;
    unsigned long compresslen = 0;
    unsigned long retlen = Decompress_CheckTotalDataHeader(inbuffer, BLOCKCHUNK_HEADERSIZE+8,&decompresslen,&compresslen);
    compresslen = retlen;
    unsigned long maxtotalrseultbuffer = *outbufferlen;
    if(0 != retlen)
    {
      /**
       * Begin to Decompress Data Block
      */
      unsigned long currnet_decompresslen = 0;
      while(currnet_decompresslen < decompresslen)
      {
        unsigned long len = 0;
        unsigned long resultlen = Decompress_LZ77DataBlock(&inbuffer[compresslen],inbufferlen - compresslen,&outbuffer[currnet_decompresslen],maxtotalrseultbuffer,&len);
        if(resultlen != 0)
        {
          currnet_decompresslen += resultlen;
          compresslen += len;
          maxtotalrseultbuffer -= resultlen;
        }
        else
        {
          printf("Decompress Error\n");
          return 0;
        }
      }
      *outbufferlen = currnet_decompresslen;
    }
    else
    {
      return 0x0;
    }
    return 0x01;
}

void usage(void) {
  printf("6unpack: uncompress 6pack archive\n");
  printf("Copyright (C) Ariya Hidayat\n");
  printf("\n");
  printf("Usage: 6unpack archive-file\n");
  printf("\n");
}

int main(int argc, char** argv) {
  int i;
  const char* archive_file;

  /* show help with no argument at all*/
  if (argc == 1) {
    usage();
    return 0;
  }

  /* check for help on usage */
  for (i = 1; i <= argc; i++)
    if (argv[i])
      if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
        usage();
        return 0;
      }

  /* check for version information */
  for (i = 1; i <= argc; i++)
    if (argv[i])
      if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")) {
        printf("6unpack: high-speed file compression tool\n");
        printf("Version %s (using FastLZ %s)\n", SIXPACK_VERSION_STRING, FASTLZ_VERSION_STRING);
        printf("Copyright (C) Ariya Hidayat\n");
        printf("\n");
        return 0;
      }

  /* needs at least two arguments */
  if (argc <= 1) {
    usage();
    return 0;
  }

  archive_file = argv[1];

  unsigned char compressedbuffer[] = {
0x89,0x36,0x50,0x4B,0x0D,0x0A,0x1A,0x0A,0x01,0x00,0x00,0x00,0x2F,0x05,0x00,0x00,
0xCB,0x01,0x34,0x17,0x00,0x00,0x00,0x00,0x11,0x00,0x01,0x00,0x76,0x03,0x00,0x00,
0xEE,0x11,0x37,0x33,0x00,0x04,0x00,0x00,0x3F,0x65,0x36,0x35,0x72,0x74,0x37,0x79,
0x75,0x38,0x2C,0x2E,0x6C,0x6F,0x69,0x6B,0x75,0x74,0x6A,0x79,0x72,0x68,0x74,0x67,
0x72,0x66,0x65,0x64,0x77,0x72,0x74,0x79,0x79,0x11,0x75,0x69,0x6F,0x39,0x38,0x39,
0x69,0x37,0x75,0x79,0x36,0x35,0x74,0x72,0x65,0x64,0x65,0x65,0x20,0x15,0x0F,0x6A,
0x75,0x6B,0x69,0x6F,0x70,0x3B,0x30,0x6F,0x39,0x69,0x38,0x37,0x75,0x36,0x79,0x20,
0x19,0x10,0x34,0x65,0x33,0x77,0x64,0x68,0x6A,0x6B,0x6C,0x70,0x5B,0x27,0x2D,0x70,
0x30,0x39,0x6F,0x40,0x19,0x60,0x33,0x20,0x49,0x06,0x68,0x6E,0x6A,0x6D,0x6B,0x2C,
0x6C,0xE0,0x00,0x33,0x01,0x75,0x37,0x60,0x19,0x20,0x62,0x09,0x6A,0x69,0x75,0x6B,
0x6F,0x39,0x30,0x39,0x38,0x37,0x40,0x5E,0x05,0x34,0x66,0x72,0x65,0x68,0x6A,0x20,
0x73,0x09,0x6B,0x6F,0x6C,0x70,0x30,0x2D,0x30,0x5D,0x3D,0x5B,0x20,0x4B,0x20,0x80,
0x1C,0x69,0x75,0x36,0x72,0x35,0x74,0x66,0x6A,0x6B,0x27,0x6F,0x69,0x77,0x64,0x74,
0x6B,0x6C,0x27,0x2E,0x2C,0x6F,0x68,0x72,0x64,0x74,0x66,0x79,0x67,0x75,0x40,0x75,
0x10,0x6C,0x6B,0x75,0x3B,0x6C,0x6B,0x6A,0x68,0x66,0x64,0x73,0x72,0x73,0x72,0x72,
0x64,0x66,0x80,0x16,0x00,0x3B,0x20,0x14,0x12,0x38,0x75,0x39,0x70,0x69,0x5B,0x6F,
0x62,0x74,0x61,0x69,0x6E,0x69,0x6E,0x67,0x3B,0x27,0x3B,0x2F,0x40,0x29,0x07,0x67,
0x74,0x66,0x64,0x72,0x35,0x72,0x36,0x20,0xFC,0x09,0x38,0x75,0x69,0x70,0x5B,0x5D,
0x0A,0x22,0x3A,0x3F,0xA0,0x18,0x08,0x72,0x65,0x33,0x34,0x35,0x36,0x37,0x38,0x39,
0x20,0xE4,0x0A,0x2C,0x6D,0x6E,0x62,0x76,0x63,0x64,0x73,0x61,0x77,0x32,0x80,0x14,
0x01,0x6F,0x6C,0x60,0x12,0x04,0x66,0x64,0x65,0x73,0x77,0x60,0x11,0x21,0x22,0x09,
0x3B,0x27,0x6B,0x6A,0x74,0x79,0x72,0x34,0x33,0x32,0x80,0x37,0x20,0xBB,0x00,0x70,
0x41,0x1C,0x40,0xE8,0x05,0x74,0x35,0x72,0x34,0x74,0x74,0x20,0xD4,0x01,0x6F,0x70,
0x20,0xF2,0x40,0x56,0x02,0x75,0x6F,0x70,0x20,0xAA,0x01,0x67,0x79,0x40,0x68,0x40,
0x0F,0x02,0x39,0x70,0x5B,0x20,0xA7,0x20,0x11,0x01,0x72,0x65,0x60,0x3E,0x00,0x69,
0x20,0x77,0x03,0x5B,0x3B,0x3B,0x27,0x40,0x13,0x06,0x79,0x74,0x72,0x6F,0x5B,0x6A,
0x68,0x20,0x39,0x02,0x34,0x33,0x34,0x60,0x2A,0x00,0x5D,0x80,0x2B,0x00,0x66,0x20,
0x12,0x20,0x52,0x00,0x6C,0xE0,0x00,0x0D,0x01,0x38,0x39,0x20,0xC3,0x0A,0x27,0x3B,
0x6C,0x2C,0x6B,0x6D,0x6A,0x6E,0x68,0x62,0x67,0x21,0x0A,0x02,0x77,0x65,0x33,0x20,
0xC5,0xE0,0x02,0xC4,0x01,0x65,0x77,0x20,0x0F,0x20,0x6E,0x00,0x27,0x60,0xC2,0x07,
0x63,0x73,0x77,0x30,0x2D,0x5B,0x27,0x3B,0x20,0x13,0x20,0x23,0x20,0x6D,0x01,0x62,
0x67,0x40,0xD6,0x20,0x0E,0x20,0x22,0x40,0x0F,0x20,0x65,0x04,0x36,0x37,0x69,0x6F,
0x5D,0xC0,0x00,0x04,0x74,0x32,0x6E,0x20,0x68,0x21,0x06,0x1F,0x3B,0x74,0x68,0x6C,
0x70,0x33,0x34,0x32,0x32,0x33,0x72,0x69,0x3B,0x6F,0x6A,0x66,0x6C,0x68,0x67,0x77,
0x65,0x3B,0x2F,0x6C,0x6D,0x2E,0x2C,0x77,0x6E,0x6A,0x64,0x77,0x1F,0x61,0x6F,0x6C,
0x2E,0x32,0x6D,0x33,0x2C,0x65,0x66,0x77,0x76,0x20,0x65,0x31,0x32,0x6C,0x2E,0x2C,
0x6D,0x77,0x20,0x64,0x71,0x6E,0x63,0x62,0x32,0x33,0x65,0x2C,0x6D,0x07,0x20,0x64,
0x77,0x6E,0x72,0x76,0x66,0x69,0x20,0x0C,0x07,0x6B,0x6C,0x6D,0x77,0x64,0x6E,0x6A,
0x66,0x21,0xF6,0x00,0x6F,0x40,0x0D,0x04,0x64,0x6A,0x72,0x79,0x0A,0x21,0x5B,0x00,
0x77,0x21,0xD5,0x03,0x6E,0x73,0x20,0x63,0x20,0x92,0x19,0x71,0x33,0x71,0x72,0x6B,
0x34,0x62,0x34,0x33,0x79,0x72,0x6F,0x6B,0x6C,0x20,0x2C,0x6E,0x68,0x67,0x20,0x71,
0x39,0x69,0x6F,0x6A,0x6B,0x21,0xC3,0x04,0x69,0x6A,0x73,0x7A,0x77,0x22,0xC2,0x01,
0x79,0x67,0x40,0xEE,0x01,0x79,0x37,0x22,0x0D,0x22,0x04,0x04,0x72,0x75,0x69,0x6A,
0x6B,0x21,0xB8,0x21,0x25,0x81,0xAE,0x02,0x67,0x74,0x79,0x41,0xCA,0x03,0x6A,0x68,
0x75,0x2D,0xC0,0xDA,0x00,0x74,0x40,0x23,0x20,0xE1,0x04,0x72,0x67,0x66,0x75,0x6B,
0x21,0xE3,0x03,0x66,0x63,0x6C,0x6B,0x20,0x21,0x20,0x5D,0x01,0x79,0x6F,0x41,0xF5,
0x20,0x21,0x21,0x1B,0x00,0x2E,0x21,0x16,0x04,0x68,0x79,0x75,0x30,0x70,0x40,0x07,
0x20,0x2C,0x20,0x29,0x21,0x59,0x01,0x72,0x69,0x20,0x89,0x00,0x6A,0x20,0x0E,0x22,
0xAA,0x20,0x0D,0x01,0x6B,0x69,0x20,0x24,0x20,0x16,0x00,0x65,0x23,0x48,0x60,0x08,
0x21,0xD5,0x00,0x79,0x20,0x89,0x20,0x12,0x22,0x21,0x01,0x72,0x74,0x20,0x07,0x02,
0x77,0x61,0x65,0x60,0x41,0x61,0x67,0x20,0x2B,0x20,0x0C,0x20,0x15,0x00,0x69,0x20,
0x06,0x20,0x0C,0x04,0x64,0x75,0x6A,0x67,0x0A,0x60,0x07,0x22,0x27,0x60,0xC0,0x01,
0x78,0x73,0x43,0x67,0x01,0x75,0x6A,0x20,0x5B,0x0D,0x64,0x73,0x35,0x66,0x67,0x68,
0x76,0x6A,0x62,0x6E,0x6B,0x6D,0x6C,0x2C,0x20,0xF6,0x02,0x70,0x6B,0x6C,0x20,0x5F,
0x42,0x64,0x11,0x35,0x32,0x35,0x31,0x33,0x32,0x65,0x76,0x68,0x62,0x65,0x67,0x76,
0x68,0x6A,0x62,0x6B,0x6E,0x20,0x33,0x04,0x67,0x68,0x61,0x65,0x73,0xE3,0x00,0x1A,
0x06,0x61,0x77,0x73,0x65,0x64,0x72,0x66,0x23,0xD1,0x21,0x22,0x20,0xF1,0x21,0xBD,
0x0C,0x35,0x36,0x75,0x69,0x67,0x68,0x6A,0x6E,0x66,0x67,0x76,0x68,0x62,0x11,0x00,
0x01,0x00,0x2E,0x01,0x00,0x00,0x69,0x5E,0xA7,0x6A,0x2F,0x01,0x00,0x00,0x32,0x6A,
0x6E,0x73,0x65,0x72,0x64,0x66,0x79,0x67,0x75,0x68,0x6A,0x61,0x65,0x72,0x74,0x72,
0x64,0x74,0x20,0x0C,0x06,0x76,0x68,0x62,0x5D,0x0A,0x65,0x73,0x80,0x0C,0x1F,0x75,
0x68,0x66,0x64,0x73,0x78,0x67,0x72,0x66,0x62,0x6A,0x6D,0x77,0x64,0x62,0x76,0x66,
0x6A,0x6B,0x77,0x71,0x68,0x6B,0x66,0x75,0x6A,0x68,0x71,0x65,0x6C,0x66,0x68,0x1F,
0x6C,0x71,0x2E,0x69,0x32,0x77,0x65,0x6A,0x6C,0x2E,0x66,0x6A,0x32,0x20,0x20,0x20,
0x66,0x65,0x68,0x6A,0x6C,0x69,0x32,0x20,0x6A,0x68,0x66,0x20,0x6B,0x6C,0x2E,0x63,
0x1C,0x6E,0x6D,0x2E,0x6B,0x77,0x63,0x6E,0x61,0x2E,0x6B,0x6E,0x2E,0x41,0x2E,0x53,
0x4E,0x43,0x2E,0x49,0x4C,0x48,0x4A,0x49,0x4A,0x46,0x32,0x4C,0x49,0x4B,0x20,0x05,
0x1F,0x48,0x46,0x55,0x55,0x4F,0x33,0x20,0x49,0x32,0x55,0x45,0x50,0x4F,0x49,0x39,
0x4C,0x34,0x54,0x47,0x54,0x52,0x20,0x55,0x20,0x55,0x48,0x59,0x33,0x4F,0x4C,0x49,
0x33,0x04,0x51,0x41,0x59,0x48,0x50,0x20,0x58,0x0E,0x47,0x54,0x20,0x20,0x50,0x4C,
0x4F,0x32,0x79,0x74,0x20,0x20,0x32,0x79,0x68,0x20,0x05,0x40,0x13,0x0B,0x46,0x20,
0x2F,0x3B,0x68,0x32,0x33,0x34,0x3B,0x4F,0x59,0x48,0x20,0x0C,0x20,0x00,0x04,0x3B,
0x2F,0x54,0x68,0x67,0x20,0x07,0x03,0x2F,0x3B,0x59,0x48,0x20,0x06,0x1F,0x20,0x54,
0x2F,0x47,0x79,0x68,0x3B,0x20,0x20,0x74,0x67,0x2E,0x6F,0x67,0x68,0x32,0x6F,0x75,
0x6B,0x65,0x68,0x73,0x65,0x68,0x6E,0x62,0x66,0x64,0x6D,0x7A,0x78,0x20,0x09,0x63,
0x76,0x61,0x71,0x75,0x69,0x77,0x65,0x34,0x66,0x20,0x53,0x1F,0x32,0x69,0x71,0x6C,
0x33,0x34,0x72,0x6B,0x64,0x6E,0x6A,0x6D,0x62,0x76,0x68,0x6A,0x6D,0x67,0x66,0x69,
0x33,0x67,0x66,0x6C,0x2C,0x76,0x6A,0x6E,0x2C,0x6B,0x76,0x5D
  };

  unsigned char RawDataResult[] = {
0x65,0x36,0x35,0x72,0x74,0x37,0x79,0x75,0x38,0x2C,0x2E,0x6C,0x6F,0x69,0x6B,0x75,
0x74,0x6A,0x79,0x72,0x68,0x74,0x67,0x72,0x66,0x65,0x64,0x77,0x72,0x74,0x79,0x79,
0x75,0x69,0x6F,0x39,0x38,0x39,0x69,0x37,0x75,0x79,0x36,0x35,0x74,0x72,0x65,0x64,
0x65,0x65,0x72,0x74,0x79,0x6A,0x75,0x6B,0x69,0x6F,0x70,0x3B,0x30,0x6F,0x39,0x69,
0x38,0x37,0x75,0x36,0x79,0x35,0x74,0x72,0x34,0x65,0x33,0x77,0x64,0x68,0x6A,0x6B,
0x6C,0x70,0x5B,0x27,0x2D,0x70,0x30,0x39,0x6F,0x69,0x38,0x37,0x75,0x79,0x36,0x35,
0x74,0x72,0x66,0x65,0x64,0x68,0x6E,0x6A,0x6D,0x6B,0x2C,0x6C,0x69,0x6F,0x70,0x3B,
0x30,0x6F,0x39,0x69,0x38,0x75,0x37,0x79,0x36,0x35,0x74,0x72,0x65,0x64,0x77,0x6A,
0x69,0x75,0x6B,0x6F,0x39,0x30,0x39,0x38,0x37,0x36,0x35,0x74,0x72,0x34,0x66,0x72,
0x65,0x68,0x6A,0x79,0x75,0x69,0x6B,0x6F,0x6C,0x70,0x30,0x2D,0x30,0x5D,0x3D,0x5B,
0x2D,0x70,0x30,0x6F,0x39,0x38,0x69,0x75,0x36,0x72,0x35,0x74,0x66,0x6A,0x6B,0x27,
0x6F,0x69,0x77,0x64,0x74,0x6B,0x6C,0x27,0x2E,0x2C,0x6F,0x68,0x72,0x64,0x74,0x66,
0x79,0x67,0x75,0x68,0x6A,0x6B,0x6C,0x6C,0x6B,0x75,0x3B,0x6C,0x6B,0x6A,0x68,0x66,
0x64,0x73,0x72,0x73,0x72,0x72,0x64,0x66,0x67,0x75,0x68,0x6A,0x6B,0x6C,0x3B,0x3B,
0x6C,0x6B,0x38,0x75,0x39,0x70,0x69,0x5B,0x6F,0x62,0x74,0x61,0x69,0x6E,0x69,0x6E,
0x67,0x3B,0x27,0x3B,0x2F,0x6C,0x6B,0x6A,0x68,0x67,0x74,0x66,0x64,0x72,0x35,0x72,
0x36,0x74,0x37,0x79,0x38,0x75,0x69,0x70,0x5B,0x5D,0x0A,0x22,0x3A,0x3F,0x6C,0x6B,
0x6A,0x68,0x67,0x74,0x66,0x72,0x65,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x6F,0x70,
0x3B,0x2C,0x6D,0x6E,0x62,0x76,0x63,0x64,0x73,0x61,0x77,0x32,0x33,0x34,0x35,0x36,
0x37,0x38,0x6F,0x6C,0x2C,0x6D,0x6E,0x62,0x76,0x66,0x64,0x65,0x73,0x77,0x33,0x34,
0x35,0x36,0x37,0x75,0x69,0x6F,0x3B,0x27,0x6B,0x6A,0x74,0x79,0x72,0x34,0x33,0x32,
0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x2D,0x30,0x70,0x6F,0x39,0x69,0x38,0x75,0x37,
0x79,0x36,0x74,0x35,0x72,0x34,0x74,0x74,0x79,0x75,0x69,0x6F,0x70,0x74,0x72,0x65,
0x35,0x36,0x37,0x38,0x75,0x6F,0x70,0x6B,0x6A,0x68,0x67,0x79,0x74,0x66,0x72,0x65,
0x35,0x36,0x37,0x38,0x39,0x70,0x5B,0x3B,0x6C,0x6B,0x6A,0x68,0x67,0x72,0x65,0x34,
0x35,0x36,0x37,0x38,0x69,0x39,0x6F,0x70,0x5B,0x3B,0x3B,0x27,0x6C,0x6B,0x6A,0x68,
0x79,0x74,0x72,0x6F,0x5B,0x6A,0x68,0x74,0x72,0x65,0x34,0x33,0x34,0x37,0x38,0x39,
0x70,0x5B,0x5D,0x3B,0x6C,0x6B,0x6A,0x68,0x67,0x66,0x74,0x72,0x65,0x69,0x6F,0x70,
0x6C,0x3B,0x6C,0x6B,0x6A,0x68,0x67,0x66,0x74,0x72,0x38,0x39,0x5B,0x5D,0x0A,0x27,
0x3B,0x6C,0x2C,0x6B,0x6D,0x6A,0x6E,0x68,0x62,0x67,0x66,0x64,0x73,0x77,0x65,0x33,
0x36,0x37,0x38,0x6F,0x70,0x3B,0x2C,0x6D,0x6E,0x62,0x76,0x63,0x64,0x73,0x65,0x77,
0x36,0x37,0x38,0x39,0x70,0x5B,0x27,0x2C,0x6D,0x6E,0x62,0x76,0x63,0x73,0x77,0x30,
0x2D,0x5B,0x27,0x3B,0x36,0x37,0x38,0x6F,0x70,0x3B,0x6C,0x6B,0x6A,0x62,0x67,0x76,
0x66,0x64,0x65,0x36,0x37,0x38,0x39,0x70,0x5B,0x3B,0x6C,0x6B,0x6A,0x68,0x67,0x66,
0x36,0x37,0x69,0x6F,0x5D,0x5D,0x5D,0x5D,0x5D,0x5D,0x5D,0x5D,0x5D,0x74,0x32,0x6E,
0x20,0x68,0x32,0x33,0x34,0x3B,0x74,0x68,0x6C,0x70,0x33,0x34,0x32,0x32,0x33,0x72,
0x69,0x3B,0x6F,0x6A,0x66,0x6C,0x68,0x67,0x77,0x65,0x3B,0x2F,0x6C,0x6D,0x2E,0x2C,
0x77,0x6E,0x6A,0x64,0x77,0x61,0x6F,0x6C,0x2E,0x32,0x6D,0x33,0x2C,0x65,0x66,0x77,
0x76,0x20,0x65,0x31,0x32,0x6C,0x2E,0x2C,0x6D,0x77,0x20,0x64,0x71,0x6E,0x63,0x62,
0x32,0x33,0x65,0x2C,0x6D,0x20,0x64,0x77,0x6E,0x72,0x76,0x66,0x69,0x32,0x33,0x65,
0x6B,0x6C,0x6D,0x77,0x64,0x6E,0x6A,0x66,0x68,0x6A,0x79,0x6F,0x33,0x65,0x6B,0x6C,
0x64,0x6A,0x72,0x79,0x0A,0x66,0x64,0x65,0x77,0x68,0x6A,0x6B,0x6E,0x73,0x20,0x63,
0x62,0x67,0x76,0x71,0x33,0x71,0x72,0x6B,0x34,0x62,0x34,0x33,0x79,0x72,0x6F,0x6B,
0x6C,0x20,0x2C,0x6E,0x68,0x67,0x20,0x71,0x39,0x69,0x6F,0x6A,0x6B,0x67,0x74,0x66,
0x69,0x6A,0x73,0x7A,0x77,0x35,0x72,0x74,0x79,0x67,0x64,0x73,0x77,0x65,0x79,0x37,
0x75,0x68,0x6A,0x68,0x66,0x64,0x72,0x75,0x69,0x6A,0x6B,0x6D,0x6E,0x62,0x67,0x66,
0x74,0x6F,0x6C,0x2C,0x6D,0x6E,0x62,0x67,0x74,0x79,0x70,0x3B,0x2C,0x6D,0x6A,0x68,
0x75,0x2D,0x70,0x5B,0x3B,0x6C,0x6B,0x6A,0x68,0x67,0x74,0x75,0x69,0x6A,0x6B,0x68,
0x67,0x66,0x72,0x67,0x66,0x75,0x6B,0x6E,0x62,0x76,0x66,0x63,0x6C,0x6B,0x2C,0x6D,
0x6A,0x6E,0x68,0x67,0x79,0x6F,0x3B,0x2C,0x6D,0x6E,0x68,0x67,0x74,0x5B,0x27,0x3B,
0x2E,0x6C,0x6B,0x6A,0x68,0x79,0x75,0x30,0x70,0x6C,0x6B,0x6A,0x68,0x67,0x66,0x72,
0x6B,0x6E,0x62,0x67,0x66,0x64,0x72,0x69,0x6F,0x6B,0x6C,0x6A,0x67,0x66,0x72,0x75,
0x69,0x6B,0x66,0x64,0x72,0x6B,0x69,0x6B,0x6A,0x68,0x67,0x66,0x64,0x65,0x37,0x79,
0x75,0x6A,0x68,0x67,0x66,0x64,0x35,0x36,0x37,0x79,0x75,0x68,0x6A,0x67,0x66,0x64,
0x33,0x34,0x35,0x72,0x74,0x67,0x66,0x64,0x77,0x61,0x65,0x6C,0x6B,0x6A,0x68,0x67,
0x70,0x3B,0x6C,0x6B,0x6A,0x68,0x67,0x66,0x6C,0x6B,0x6A,0x67,0x66,0x64,0x69,0x6C,
0x6B,0x6A,0x68,0x67,0x66,0x64,0x75,0x6A,0x67,0x0A,0x68,0x67,0x66,0x64,0x75,0x74,
0x79,0x75,0x68,0x6A,0x68,0x66,0x64,0x78,0x73,0x65,0x72,0x74,0x79,0x75,0x6A,0x64,
0x72,0x6B,0x64,0x73,0x35,0x66,0x67,0x68,0x76,0x6A,0x62,0x6E,0x6B,0x6D,0x6C,0x2C,
0x69,0x6F,0x6A,0x70,0x6B,0x6C,0x35,0x36,0x37,0x38,0x39,0x30,0x2D,0x35,0x32,0x35,
0x31,0x33,0x32,0x65,0x76,0x68,0x62,0x65,0x67,0x76,0x68,0x6A,0x62,0x6B,0x6E,0x74,
0x79,0x75,0x67,0x68,0x61,0x65,0x73,0x72,0x64,0x74,0x66,0x79,0x67,0x75,0x68,0x6A,
0x61,0x77,0x73,0x65,0x64,0x72,0x66,0x74,0x67,0x72,0x74,0x79,0x67,0x75,0x69,0x6A,
0x32,0x33,0x34,0x35,0x36,0x75,0x69,0x67,0x68,0x6A,0x6E,0x66,0x67,0x76,0x68,0x62,
0x6A,0x6E,0x73,0x65,0x72,0x64,0x66,0x79,0x67,0x75,0x68,0x6A,0x61,0x65,0x72,0x74,
0x72,0x64,0x74,0x66,0x79,0x67,0x76,0x68,0x62,0x5D,0x0A,0x65,0x73,0x72,0x64,0x74,
0x66,0x79,0x67,0x75,0x68,0x66,0x64,0x73,0x78,0x67,0x72,0x66,0x62,0x6A,0x6D,0x77,
0x64,0x62,0x76,0x66,0x6A,0x6B,0x77,0x71,0x68,0x6B,0x66,0x75,0x6A,0x68,0x71,0x65,
0x6C,0x66,0x68,0x6C,0x71,0x2E,0x69,0x32,0x77,0x65,0x6A,0x6C,0x2E,0x66,0x6A,0x32,
0x20,0x20,0x20,0x66,0x65,0x68,0x6A,0x6C,0x69,0x32,0x20,0x6A,0x68,0x66,0x20,0x6B,
0x6C,0x2E,0x63,0x6E,0x6D,0x2E,0x6B,0x77,0x63,0x6E,0x61,0x2E,0x6B,0x6E,0x2E,0x41,
0x2E,0x53,0x4E,0x43,0x2E,0x49,0x4C,0x48,0x4A,0x49,0x4A,0x46,0x32,0x4C,0x49,0x4B,
0x4A,0x46,0x32,0x48,0x46,0x55,0x55,0x4F,0x33,0x20,0x49,0x32,0x55,0x45,0x50,0x4F,
0x49,0x39,0x4C,0x34,0x54,0x47,0x54,0x52,0x20,0x55,0x20,0x55,0x48,0x59,0x33,0x4F,
0x4C,0x49,0x33,0x51,0x41,0x59,0x48,0x50,0x32,0x20,0x20,0x47,0x54,0x20,0x20,0x50,
0x4C,0x4F,0x32,0x79,0x74,0x20,0x20,0x32,0x79,0x68,0x74,0x20,0x20,0x20,0x20,0x47,
0x54,0x46,0x20,0x2F,0x3B,0x68,0x32,0x33,0x34,0x3B,0x4F,0x59,0x48,0x54,0x46,0x20,
0x20,0x20,0x20,0x3B,0x2F,0x54,0x68,0x67,0x20,0x20,0x20,0x2F,0x3B,0x59,0x48,0x20,
0x20,0x20,0x20,0x54,0x2F,0x47,0x79,0x68,0x3B,0x20,0x20,0x74,0x67,0x2E,0x6F,0x67,
0x68,0x32,0x6F,0x75,0x6B,0x65,0x68,0x73,0x65,0x68,0x6E,0x62,0x66,0x64,0x6D,0x7A,
0x78,0x20,0x63,0x76,0x61,0x71,0x75,0x69,0x77,0x65,0x34,0x66,0x79,0x68,0x74,0x32,
0x69,0x71,0x6C,0x33,0x34,0x72,0x6B,0x64,0x6E,0x6A,0x6D,0x62,0x76,0x68,0x6A,0x6D,
0x67,0x66,0x69,0x33,0x67,0x66,0x6C,0x2C,0x76,0x6A,0x6E,0x2C,0x6B,0x76,0x5D
  };

  unsigned long decompresslen = 65535;
  if(0x01 == Decompress_TotalDataBlock(compressedbuffer,sizeof(compressedbuffer),Decompress_TestResultBuffer,&decompresslen))
  {
    printf("decompresslen:%ld\n",decompresslen);
    for(i=0;i<decompresslen;i++)
    {
      if(i%16 == 0)
      {
        printf("\n");
      }
      printf("0x%02X,",Decompress_TestResultBuffer[i]);
    }
    printf("\n");

    if(decompresslen != sizeof(RawDataResult))
    {
      printf("Decompress Failure\n");
    }
    else
    {
      for(i = 0;i<decompresslen;i++)
      {
        if(Decompress_TestResultBuffer[i] != RawDataResult[i])
        {
          printf("Decompress Failure\n");
        }
      }
      printf("Decompress Success\n");
    }
  }

  return 0;
}
