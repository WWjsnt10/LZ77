cmake_minimum_required(VERSION 3.6)
project(GmSSL)




set(Code_Path  "../../GMSSL")

include_directories("../../include"
                    "${Code_Path}/SM4")

set(src
	${Code_Path}/MD5/md5.c
)

set(testsrc
    ./md5test.c
    ${Code_Path}/hex.c
)

add_executable(md5test ${src} ${testsrc})	




