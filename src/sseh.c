#include <stdio.h>
#include <stdlib.h>
#include "sse.h"

#define FEN1 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 3"
#define FEN3 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 1 12"
#define FEN4 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 3"



int main() {

    Init();

    BOARD pos[1];

    ParseFEN(FEN1, pos);
    PrintBoard(pos);

    ParseFEN(FEN4, pos);
    PrintBoard(pos);

    ASSERT(CheckBoard(pos));


    return 0;
}