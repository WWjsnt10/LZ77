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


uint8_t encrypt_data[48] = {
	0x66,0xBC,0xA6,0x04,0x94,0xF5,0x66,0xA6,0x31,0xD2,0xA2,0xAA,0xFC,0x99,0xAE,0x9B,
	0x71,0x73,0xB5,0x21,0x77,0x9D,0xE3,0x17,0xF6,0x75,0x5D,0x35,0x18,0x22,0x28,0x64,
	0x95,0xD8,0x4E,0x6B,0xF1,0xEE,0xE2,0x45,0x7B,0xBA,0xE1,0x2D,0xCD,0xDD,0x52,0xC3
};

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

	if (sm4_cbc_decrypt_init(&cbc_ctx, key, iv) != 1) {
		fprintf(stderr, "%s %d: error\n", __FILE__, __LINE__);
		return 1;
	}
	printf("Begin to read\n");

	for(int i=0;i<48;i++)
	{
		inbuf[i] = encrypt_data[i];
	}
	inlen = 48;
	if(inlen > 0) {
		if (sm4_cbc_decrypt_update(&cbc_ctx, inbuf, inlen, outbuf, &outlen) != 1) {
			fprintf(stderr, "%s %d: error\n", __FILE__, __LINE__);
			return 1;
		}
		//fwrite(outbuf, 1, outlen, stdout);
		for(int i=0;i<outlen;i++)
		{
			printf("0x%02X,",outbuf[i]);
		}

		printf("\n");

		/*if (sm4_cbc_decrypt_update(&cbc_ctx, &inbuf[outlen], 0 , outbuf, &outlen) != 1) {
			fprintf(stderr, "%s %d: error\n", __FILE__, __LINE__);
			return 1;
		}
		//fwrite(outbuf, 1, outlen, stdout);
		for(int i=0;i<outlen;i++)
		{
			printf("0x%02X,",outbuf[i]);
		}
		printf("\n");*/
	}
	if (sm4_cbc_decrypt_finish(&cbc_ctx, outbuf, &outlen) != 1) {
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
