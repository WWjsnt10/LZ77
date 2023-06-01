#ifndef UNPACK_H
#define UNPACK_H

#if defined(__cplusplus)
extern "C" {
#endif


extern unsigned long Decompress_DataBlock(unsigned char* inbuffer, int chunk_id, int chunk_options, unsigned long chunk_size, unsigned long chunk_checksum, unsigned long chunk_extra,unsigned char* result);
extern unsigned long Decompress_LZ77DataBlock(unsigned char* inbuffer, unsigned long inbufferlen , unsigned char* result, unsigned long maxout, unsigned long* compresslen);
extern int Decompress_DetectMagic(unsigned char* magic, unsigned long magiclen);
extern unsigned long Decompress_CheckTotalDataHeader(unsigned char* inbuffer, unsigned long inbufferlen, unsigned long* decompresslen, unsigned long* compresslen);

#if defined(__cplusplus)
}
#endif



#endif