CFLAGS?=-Wall -std=c90 -g

all: 6pack 6unpack

6pack: 6pack.c checksum.c 6packmain.c ../fastlz.c
	$(CC) -o 6pack $(CFLAGS) -I.. 6pack.c checksum.c 6packmain.c ../fastlz.c

6unpack: 6unpack.c checksum.c 6unpackmain.c ../fastlz.c
	$(CC) -o 6unpack $(CFLAGS) -I.. 6unpack.c checksum.c 6unpackmain.c ../fastlz.c

clean :
	$(RM) 6pack 6unpack *.o
