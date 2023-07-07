cmake_minimum_required(VERSION 3.6)
project(GmSSL)




set(Code_Path  "../../GMSSL")

include_directories("../../include"
                    "${Code_Path}/SM4")

set(src
	${Code_Path}/zuc/zuc.c
	${Code_Path}/zuc/zuc_modes.c

    ${Code_Path}/debug.c
    ${Code_Path}/hex.c
)

set(testsrc
    ./demo_zuc.c
)

add_executable(demo_zuc ${src} ${testsrc})	




