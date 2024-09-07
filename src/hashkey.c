#include "sse.h"



U64 GeneratePosKey(const BOARD *pos) {

    int sq = 0;
    U64 finalKey = 0;
    int piece = EMPTY;

    for(sq = 0; sq < BOARD_SIZE; ++sq) {
        piece = pos->pieces[sq];
        if(piece != NO_SQ && piece != EMPTY) {
            ASSERT(piece >= wP && piece <= bK);
            finalKey ^= PieceKeys[piece][sq];
        }
    }

    if(pos->side == WHITE) {
        finalKey ^= SideKey;
    }

    if(pos->enPass != NO_SQ) {
        ASSERT(pos->enPass >= 0 && pos->enPass < BOARD_SIZE);
        finalKey ^= PieceKeys[EMPTY][pos->enPass];
    }

    ASSERT(pos->castlePerm >= 0 && pos->castlePerm <= 15);

    finalKey ^= CastleKeys[pos->castlePerm];

    return finalKey;
}