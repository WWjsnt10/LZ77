cmake_minimum_required(VERSION 3.6)
project(GmSSL)




set(Code_Path  "../../GMSSL")

include_directories("../../include"
                    "${Code_Path}/SM4")

set(src
	${Code_Path}/SM9/sm9_alg.c
	${Code_Path}/SM9/sm9_key.c
	${Code_Path}/SM9/sm9_lib.c

    ${Code_Path}/SM4/sm4_common.c
    ${Code_Path}/SM4/sm4_enc.c
    ${Code_Path}/SM4/sm4_modes.c
    ${Code_Path}/SM4/sm4_setkey.c

    ${Code_Path}/gcm.c
    ${Code_Path}/gf128.c
    ${Code_Path}/block_cipher.c
    ${Code_Path}/aes.c
    ${Code_Path}/aes_modes.c

	${Code_Path}/SM3/sm3.c
	${Code_Path}/SM3/sm3_hmac.c
	${Code_Path}/SM3/sm3_kdf.c

	${Code_Path}/SM2/sm2_alg.c
	${Code_Path}/SM2/sm2_key.c
	${Code_Path}/SM2/sm2_lib.c

	${Code_Path}/sha256.c
	${Code_Path}/sha512.c
	${Code_Path}/digest.c
	${Code_Path}/hmac.c
	${Code_Path}/pbkdf2.c
	${Code_Path}/pkcs8.c
	${Code_Path}/ec.c
	${Code_Path}/asn1.c
	${Code_Path}/base64.c
	${Code_Path}/pem.c
	${Code_Path}/x509_alg.c

    ${Code_Path}/debug.c
    ${Code_Path}/hex.c
)

set(testsrc
    ./demo_sm9_encrypt.c
)

add_executable(demo_sm9_encrypt ${src} ${testsrc})	


set(testsrc1
    ./demo_sm9_keygen.c
)

add_executable(demo_sm9_keygen ${src} ${testsrc1})	


set(testsrc2
    ./demo_sm9_sign.c
)

add_executable(demo_sm9_sign ${src} ${testsrc2})	




