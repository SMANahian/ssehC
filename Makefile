all:
	gcc src/sseh.c \
	src/init.c \
	src/bitboard.c \
	src/hashkey.c \
	src/board.c \
	src/data.c \
	src/attack.c \
	src/io.c \
	-o ssehc.out