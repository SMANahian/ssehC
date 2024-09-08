#include "sse.h"

#define HASH_PIECE(pce,sq) (pos->posKey ^= (PieceKeys[(pce)][(sq)]))
#define HASH_CA (pos->posKey ^= (CastleKeys[(pos->castlePerm)]))
#define HASH_SIDE (pos->posKey ^= (SideKey))
#define HASH_EP (pos->posKey ^= (PieceKeys[EMPTY][(pos->enPass)]))

const int CastlePerm[120] = {
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};


static void ClearPiece(const int sq, BOARD *pos) {

    ASSERT(SquareOnBoard(sq));

    int pce = pos->pieces[sq];

    ASSERT(PieceValid(pce));

    int color = PieceColor[pce];
    int index = 0;
    int t_pceNum = -1;

    HASH_PIECE(pce,sq);

    pos->pieces[sq] = EMPTY;
    pos->material[color] -= PieceValue[pce];

    if(PieceBig[pce] == TRUE) {
        pos->bigPiece[color]--;
        if(PieceMajor[pce] == TRUE) {
            pos->majorPiece[color]--;
        } else {
            pos->minorPiece[color]--;
        }
    } else {
        CLRBIT(pos->pawns[color],SQ64(sq));
        CLRBIT(pos->pawns[BOTH],SQ64(sq));
    }

    for(index = 0; index < pos->pieceNum[pce]; ++index) {
        if(pos->pieceList[pce][index] == sq) {
            t_pceNum = index;
            break;
        }
    }

    ASSERT(t_pceNum != -1);
    ASSERT(t_pceNum >= 0 && t_pceNum < 10);
    pos->pieceNum[pce]--;
    pos->pieceList[pce][t_pceNum] = pos->pieceList[pce][pos->pieceNum[pce]];

}

static void AddPiece(const int sq, BOARD *pos, const int pce) {

    ASSERT(PieceValid(pce));
    ASSERT(SquareOnBoard(sq));

    int color = PieceColor[pce];

    HASH_PIECE(pce,sq);

    pos->pieces[sq] = pce;

    if(PieceBig[pce] == TRUE) {
        pos->bigPiece[color]++;
        if(PieceMajor[pce] == TRUE) {
            pos->majorPiece[color]++;
        } else {
            pos->minorPiece[color]++;
        }
    } else {
        SETBIT(pos->pawns[color],SQ64(sq));
        SETBIT(pos->pawns[BOTH],SQ64(sq));
    }

    pos->material[color] += PieceValue[pce];
    pos->pieceList[pce][pos->pieceNum[pce]++] = sq;

}

static void MovePiece(const int from, const int to, BOARD *pos) {

    ASSERT(SquareOnBoard(from));
    ASSERT(SquareOnBoard(to));

    int index = 0;
    int pce = pos->pieces[from];
    int color = PieceColor[pce];

#ifdef DEBUG
    int t_PieceNum = FALSE;
#endif

    ASSERT(PieceValid(pce));

    HASH_PIECE(pce,from);
    pos->pieces[from] = EMPTY;

    HASH_PIECE(pce,to);
    pos->pieces[to] = pce;

    if(!PieceBig[pce]) {
        CLRBIT(pos->pawns[color],SQ64(from));
        CLRBIT(pos->pawns[BOTH],SQ64(from));
        SETBIT(pos->pawns[color],SQ64(to));
        SETBIT(pos->pawns[BOTH],SQ64(to));
    }

    for(index = 0; index < pos->pieceNum[pce]; ++index) {
        if(pos->pieceList[pce][index] == from) {
            pos->pieceList[pce][index] = to;
#ifdef DEBUG
            t_PieceNum = TRUE;
#endif
            break;
        }
    }
    ASSERT(t_PieceNum);
}

int MakeMove(BOARD *pos, int move) {

    ASSERT(CheckBoard(pos));

    int from = FROMSQ(move);
    int to = TOSQ(move);
    int side = pos->side;

    ASSERT(SquareOnBoard(from));
    ASSERT(SquareOnBoard(to));
    ASSERT(SideValid(side));
    ASSERT(PieceValid(pos->pieces[from]));

    pos->history[pos->hisPly].posKey = pos->posKey;

    if(move & MASK_EP) {
        if(side == WHITE) {
            ClearPiece(to-10,pos);
        } else {
            ClearPiece(to+10,pos);
        }
    } else if(move & MASK_CA) {
        switch(to) {
            case C1: MovePiece(A1,D1,pos); break;
            case C8: MovePiece(A8,D8,pos); break;
            case G1: MovePiece(H1,F1,pos); break;
            case G8: MovePiece(H8,F8,pos); break;
            default: ASSERT(FALSE); break;
        }
    }

    if(pos->enPass != NO_SQ) HASH_EP;
    HASH_CA;

    pos->history[pos->hisPly].move = move;
    pos->history[pos->hisPly].fiftyMove = pos->fiftyMove;
    pos->history[pos->hisPly].enPass = pos->enPass;
    pos->history[pos->hisPly].castlePerm = pos->castlePerm;

    pos->castlePerm &= CastlePerm[from];
    pos->castlePerm &= CastlePerm[to];
    pos->enPass = NO_SQ;

    HASH_CA;

    int captured = CAPTURED(move);
    pos->fiftyMove++;

    if(captured != EMPTY) {
        ASSERT(PieceValid(captured));
        ClearPiece(to,pos);
        pos->fiftyMove = 0;
    }

    pos->hisPly++;
    pos->ply++;

    if(PiecePawn[pos->pieces[from]]) {
        pos->fiftyMove = 0;
        if(move & MASK_PS) {
            if(side == WHITE) {
                pos->enPass = from+10;
                ASSERT(Rank[pos->enPass] == RANK_3);
            } else {
                pos->enPass = from-10;
                ASSERT(Rank[pos->enPass] == RANK_6);
            }
            HASH_EP;
        }
    }

    MovePiece(from,to,pos);

    int prPce = PROMOTED(move);
    if(prPce != EMPTY) {
        ASSERT(PieceValid(prPce) && !PiecePawn[prPce]);
        ClearPiece(to,pos);
        AddPiece(to,pos,prPce);
    }

    if(PieceKing[pos->pieces[to]]) {
        pos->KingSq[pos->side] = to;
    }

    pos->side ^= 1;
    HASH_SIDE;

    ASSERT(CheckBoard(pos));

    if(SquareAttacked(pos->KingSq[side],pos->side,pos)) {
        TakeMove(pos);
        return FALSE;
    }

    return TRUE;

}

void TakeMove(BOARD *pos) {

    ASSERT(CheckBoard(pos));

    pos->hisPly--;
    pos->ply--;

    int move = pos->history[pos->hisPly].move;
    int from = FROMSQ(move);
    int to = TOSQ(move);

    ASSERT(SquareOnBoard(from));
    ASSERT(SquareOnBoard(to));

    if(pos->enPass != NO_SQ) HASH_EP;
    HASH_CA;

    pos->castlePerm = pos->history[pos->hisPly].castlePerm;
    pos->fiftyMove = pos->history[pos->hisPly].fiftyMove;
    pos->enPass = pos->history[pos->hisPly].enPass;

    if(pos->enPass != NO_SQ) HASH_EP;
    HASH_CA;

    pos->side ^= 1;
    HASH_SIDE;

    if(MASK_EP & move) {
        if(pos->side == WHITE) {
            AddPiece(to-10,pos,bP);
        } else {
            AddPiece(to+10,pos,wP);
        }
    } else if(MASK_CA & move) {
        switch(to) {
            case C1: MovePiece(D1,A1,pos); break;
            case C8: MovePiece(D8,A8,pos); break;
            case G1: MovePiece(F1,H1,pos); break;
            case G8: MovePiece(F8,H8,pos); break;
            default: ASSERT(FALSE); break;
        }
    }

    MovePiece(to,from,pos);

    if(PieceKing[pos->pieces[from]]) {
        pos->KingSq[pos->side] = from;
    }

    int captured = CAPTURED(move);
    if(captured != EMPTY) {
        ASSERT(PieceValid(captured));
        AddPiece(to,pos,captured);
    }

    if(PROMOTED(move) != EMPTY) {
        ASSERT(PieceValid(PROMOTED(move)) && !PiecePawn[PROMOTED(move)]);
        ClearPiece(from,pos);
        AddPiece(from,pos,(PieceColor[PROMOTED(move)] == WHITE ? wP : bP));
    }

    ASSERT(CheckBoard(pos));

}
