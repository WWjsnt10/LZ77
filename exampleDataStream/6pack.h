#ifndef PACK_H
#define PACK_H


#if defined(__cplusplus)
extern "C" {
#endif


extern unsigned long LZ77_CompressDataInit(unsigned long datalen, unsigned char* outbuffer, unsigned long* outlen);
extern unsigned long LZ77_CompressDataBlock(unsigned long bytes_read, const unsigned char* const inbuffer, int level, unsigned char* result, unsigned long resultlen);


#if defined(__cplusplus)
}
#endif

#endif