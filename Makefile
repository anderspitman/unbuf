#CC := clang
CC := musl-clang
#CFLAGS := -std=c99 -Wall
CFLAGS := -static -std=c99 -Wall
SRC := unbuf.c

unbuf: $(SRC)
	$(CC) $(CFLAGS) -o unbuf $(SRC)

.PHONY: watch
watch:
	ls $(SRC) | entr make

.PHONY: clean
clean:
	$(RM) unbuf
