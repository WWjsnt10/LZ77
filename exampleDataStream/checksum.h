#ifndef CHECKSUM_H
#define CHECKSUM_H

#if defined(__cplusplus)
extern "C" {
#endif


extern unsigned long update_adler32(unsigned long checksum, const void* buf, int len);



#if defined(__cplusplus)
}
#endif

#endif