/*
 *  Copyright 2014-2022 The GmSSL Project. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the License); you may
 *  not use this file except in compliance with the License.
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gmssl/sm4.h>
#include <gmssl/rand.h>


int main(void)
{
	SM4_CBC_CTX cbc_ctx;
	unsigned char key[16] = {
		0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,
		0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,
	};
	unsigned char iv[16] = {
		0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,
		0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,
	};
	unsigned char inbuf[1024];
	unsigned char outbuf[1024 + 32];
	size_t inlen;
	size_t outlen;

	if (sm4_cbc_encrypt_init(&cbc_ctx, key, iv) != 1) {
		fprintf(stderr, "%s %d: error\n", __FILE__, __LINE__);
		return 1;
	}

	for(int i=0;i<32;i++)
	{
		inbuf[i] = 0x01+i;
	}
	inlen = 32;
	if(inlen > 0) {
		if (sm4_cbc_encrypt_update(&cbc_ctx, inbuf, inlen, outbuf, &outlen) != 1) {
			fprintf(stderr, "%s %d: error\n", __FILE__, __LINE__);
			return 1;
		}
		//fwrite(outbuf, 1, outlen, stdout);
		for(int i=0;i<outlen;i++)
		{
			printf("0x%02X,",outbuf[i]);
		}
		printf("\n");
	}
	if (sm4_cbc_encrypt_finish(&cbc_ctx, outbuf, &outlen) != 1) {
		fprintf(stderr, "%s %d: error\n", __FILE__, __LINE__);
		return 1;
	}
	//fwrite(outbuf, 1, outlen, stdout);

	for(int i=0;i<outlen;i++)
	{
		printf("0x%02X,",outbuf[i]);
	}

	return 0;
}
