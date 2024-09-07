#include <stdio.h>
#include <stdlib.h>
#include "sse.h"

#define FEN1 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 3"
#define FEN3 "8/3q4/8/8/8/4Qp2/8/8 w KQkq - 1 12"
#define FEN4 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 3"

void ShowSqAttackedBySide(const int side, const BOARD *pos) {

    int rank = 0;
    int file = 0;
    int sq = 0;
    int sq64 = 0;
    int piece = 0;

    printf("\n\nSquares attacked by: %c\n", SideChar[side]);

    for(rank = RANK_8; rank >= RANK_1; rank--) {
        for(file = FILE_A; file <= FILE_H; file++) {
            sq = FR2SQ(file, rank);
            sq64 = SQ64(sq);
            if(SquareAttacked(sq, side, pos) == TRUE) {
                piece = pos->pieces[sq];
                printf("X");
            } else {
                printf("-");
            }
        }
        printf("\n");
    }
    printf("\n\n");
} 

int main() {

    Init();

    BOARD pos[1];

    ParseFEN(FEN1, pos);
    PrintBoard(pos);

    ParseFEN(FEN3, pos);
    PrintBoard(pos);

    // ASSERT(CheckBoard(pos));

    ShowSqAttackedBySide(WHITE, pos);
    ShowSqAttackedBySide(BLACK, pos);


    return 0;
}