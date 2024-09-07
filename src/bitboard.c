#include <stdio.h>
#include "sse.h"

const int BitTable[64] = {
    63, 30, 3, 32, 25, 41, 22, 33,
    15, 50, 42, 13, 11, 53, 19, 34,
    61, 29, 2, 51, 21, 43, 45, 10,
    18, 47, 1, 54, 9, 57, 0, 35,
    62, 31, 40, 24, 14, 49, 52, 16,
    7, 26, 20, 46, 50, 44, 17, 58,
    12, 5, 8, 56, 23, 48, 4, 59,
    27, 39, 38, 55, 6, 28, 37, 60
};

int PopBit(U64 *bb) {
    U64 b = *bb ^ (*bb - 1);
    unsigned int fold = (unsigned) ((b & 0xffffffff) ^ (b >> 32));
    *bb &= (*bb - 1);
    return BitTable[(fold * 0x783a9b23) >> 26];
}

int CountBits(U64 b) {
    int r;
    for(r = 0; b; r++, b &= b - 1);
    return r;
}



void PrintBitBoard(U64 bb) {

    U64 shiftMe = 1ULL;

    int sq, sq64, rank, file, piece;

    printf("\n");
    for(rank = RANK_8; rank >= RANK_1; rank--) {
        for(file = FILE_A; file <= FILE_H; file++) {
            sq = FR2SQ(file,rank);
            sq64 = SQ64(sq);
            if((shiftMe << sq64) & bb) printf("X");
            else printf("-");
        }
        printf("\n");
    }
    printf("\n\n");
}