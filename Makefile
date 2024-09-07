all:
	gcc src/sseh.c src/init.c src/bitboard.c src/hashkey.c src/board.c src/data.c -o ssehc.out