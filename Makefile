all:
	gcc src/sseh.c \
	src/init.c \
	src/bitboard.c \
	src/hashkey.c \
	src/board.c \
	src/data.c \
	src/attack.c \
	src/io.c \
	src/genmove.c \
	src/validate.c \
	src/makemove.c \
	-o ssehc.out