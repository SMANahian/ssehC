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
    src/perft.c \
    src/misc.c \
    src/search.c \
    src/pvtable.c \
    src/evaluate.c \
    src/uci.c \
    src/xboard.c \
    src/polybook.c \
    src/polykeys.c \
    -o ssehc \
    -O3 -Wall -Wextra -Wpedantic -Wno-unused-result -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-label -Wno-unused-label -Wno-unused-local-typedefs