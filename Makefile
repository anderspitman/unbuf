CC := musl-clang
#CC := clang
#CC := gcc
CFLAGS := -static -std=c99 -Wall
#CFLAGS := -std=c99 -Wall
SRC := unbuf.c

unbuf: $(SRC)
	$(CC) $(CFLAGS) -o unbuf $(SRC)

.PHONY: clean
clean:
	$(RM) unbuf
