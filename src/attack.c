#include "sse.h"

const int NMove[8] = { -8, -19, -21, -12, 8, 19, 21, 12 };
const int RMove[4] = { -1, -10, 1, 10 };
const int BMove[4] = { -9, -11, 9, 11 };
const int KMove[8] = { -1, -10, 1, 10, -9, -11, 9, 11 };

int SquareAttacked(const int sq, const int side, const BOARD *pos) {

    int pce, t_sq, dir;

    ASSERT(SquareOnBoard(sq));
    ASSERT(SideValid(side));
    ASSERT(CheckBoard(pos));

    // pawns
    if(side == WHITE) {
        if(pos->pieces[sq - 11] == wP || pos->pieces[sq - 9] == wP) {
            return TRUE;
        }
    } else {
        if(pos->pieces[sq + 11] == bP || pos->pieces[sq + 9] == bP) {
            return TRUE;
        }
    }

    // knights
    for(int i = 0; i < 8; i++) {
        pce = pos->pieces[sq + NMove[i]];
        if(pce != OFFBOARD && IsN(pce) && PieceColor[pce] == side) {
            return TRUE;
        }
    }

    // rooks, queens
    for(int i = 0; i < 4; i++) {
        dir = RMove[i];
        t_sq = sq + dir;
        pce = pos->pieces[t_sq];
        while(pce != OFFBOARD) {
            if(pce != EMPTY) {
                if(IsRQ(pce) && PieceColor[pce] == side) {
                    return TRUE;
                }
                break;
            }
            t_sq += dir;
            pce = pos->pieces[t_sq];
        }
    }

    // bishops, queens
    for(int i = 0; i < 4; i++) {
        dir = BMove[i];
        t_sq = sq + dir;
        pce = pos->pieces[t_sq];
        while(pce != OFFBOARD) {
            if(pce != EMPTY) {
                if(IsBQ(pce) && PieceColor[pce] == side) {
                    return TRUE;
                }
                break;
            }
            t_sq += dir;
            pce = pos->pieces[t_sq];
        }
    }

    // kings
    for(int i = 0; i < 8; i++) {
        pce = pos->pieces[sq + KMove[i]];
        if(pce != OFFBOARD && IsK(pce) && PieceColor[pce] == side) {
            return TRUE;
        }
    }

    return FALSE;
}