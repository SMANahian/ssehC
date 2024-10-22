#include "sse.h"
#include <stdio.h>

const int PawnTable[64] = {
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};

const int KnightTable[64] = {
0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0		
};

const int BishopTable[64] = {
0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};

const int RookTable[64] = {
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0		
};

#define MIRROR64(sq) (Mirror64[(sq)])

int EvalPosition(const BOARD *pos) {

    int pce;
    int pceNum;
    int sq;
    int score = pos->material[WHITE] - pos->material[BLACK];

    pce = wP;
    for(pceNum = 0; pceNum < pos->pieceNum[wP]; ++pceNum) {
        sq = pos->pieceList[wP][pceNum];
        score += PawnTable[SQ64(sq)];
    }

    pce = bP;
    for(pceNum = 0; pceNum < pos->pieceNum[bP]; ++pceNum) {
        sq = pos->pieceList[bP][pceNum];
        score -= PawnTable[MIRROR64(SQ64(sq))];
    }

    pce = wN;
    for(pceNum = 0; pceNum < pos->pieceNum[wN]; ++pceNum) {
        sq = pos->pieceList[wN][pceNum];
        score += KnightTable[SQ64(sq)];
    }

    pce = bN;
    for(pceNum = 0; pceNum < pos->pieceNum[bN]; ++pceNum) {
        sq = pos->pieceList[bN][pceNum];
        score -= KnightTable[MIRROR64(SQ64(sq))];
    }

    pce = wB;
    for(pceNum = 0; pceNum < pos->pieceNum[wB]; ++pceNum) {
        sq = pos->pieceList[wB][pceNum];
        score += BishopTable[SQ64(sq)];
    }

    pce = bB;
    for(pceNum = 0; pceNum < pos->pieceNum[bB]; ++pceNum) {
        sq = pos->pieceList[bB][pceNum];
        score -= BishopTable[MIRROR64(SQ64(sq))];
    }

    pce = wR;
    for(pceNum = 0; pceNum < pos->pieceNum[wR]; ++pceNum) {
        sq = pos->pieceList[wR][pceNum];
        score += RookTable[SQ64(sq)];
    }

    pce = bR;
    for(pceNum = 0; pceNum < pos->pieceNum[bR]; ++pceNum) {
        sq = pos->pieceList[bR][pceNum];
        score -= RookTable[MIRROR64(SQ64(sq))];
    }

    return pos->side == WHITE ? score : -score;

    return 0;
}