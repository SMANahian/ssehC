all:
	gcc src/sseh.c src/init.c src/bitboard.c src/hashkey.c src/board.c -o ssehc.out