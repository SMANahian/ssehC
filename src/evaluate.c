#include "sse.h"
#include <stdio.h>

const int PawnIsolated = -10;
const int PawnPassed[8] = {0, 5, 10, 20, 35, 60, 100, 200};
const int RookOpenFile = 10;
const int RookSemiOpenFile = 5;
const int QueenOpenFile = 5;
const int QueenSemiOpenFile = 3;

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

const int KingE[64] = {	
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	,
	-10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
	-10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	
};

const int KingO[64] = {	
	0	,	5	,	5	,	-10	,	-10	,	0	,	10	,	5	,
	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,
	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70		
};

int MaterialDraw(const BOARD *pos) {

	ASSERT(CheckBoard(pos));
	
    if (!pos->pieceNum[wR] && !pos->pieceNum[bR] && !pos->pieceNum[wQ] && !pos->pieceNum[bQ]) {
	  if (!pos->pieceNum[bB] && !pos->pieceNum[wB]) {
	      if (pos->pieceNum[wN] < 3 && pos->pieceNum[bN] < 3) {  return TRUE; }
	  } else if (!pos->pieceNum[wN] && !pos->pieceNum[bN]) {
	     if (abs(pos->pieceNum[wB] - pos->pieceNum[bB]) < 2) { return TRUE; }
	  } else if ((pos->pieceNum[wN] < 3 && !pos->pieceNum[wB]) || (pos->pieceNum[wB] == 1 && !pos->pieceNum[wN])) {
	    if ((pos->pieceNum[bN] < 3 && !pos->pieceNum[bB]) || (pos->pieceNum[bB] == 1 && !pos->pieceNum[bN]))  { return TRUE; }
	  }
	} else if (!pos->pieceNum[wQ] && !pos->pieceNum[bQ]) {
        if (pos->pieceNum[wR] == 1 && pos->pieceNum[bR] == 1) {
            if ((pos->pieceNum[wN] + pos->pieceNum[wB]) < 2 && (pos->pieceNum[bN] + pos->pieceNum[bB]) < 2)	{ return TRUE; }
        } else if (pos->pieceNum[wR] == 1 && !pos->pieceNum[bR]) {
            if ((pos->pieceNum[wN] + pos->pieceNum[wB] == 0) && (((pos->pieceNum[bN] + pos->pieceNum[bB]) == 1) || ((pos->pieceNum[bN] + pos->pieceNum[bB]) == 2))) { return TRUE; }
        } else if (pos->pieceNum[bR] == 1 && !pos->pieceNum[wR]) {
            if ((pos->pieceNum[bN] + pos->pieceNum[bB] == 0) && (((pos->pieceNum[wN] + pos->pieceNum[wB]) == 1) || ((pos->pieceNum[wN] + pos->pieceNum[wB]) == 2))) { return TRUE; }
        }
    }
  return FALSE;
}

#define ENDGAME_MAT (1 * PieceValue[wR] + 2 * PieceValue[wN] + 2 * PieceValue[wP] + PieceValue[wK])

int EvalPosition(const BOARD *pos) {

    int pce;
    int pieceNum;
    int sq;
    int score = pos->material[WHITE] - pos->material[BLACK];

    if(!pos->pieceNum[wP] && !pos->pieceNum[bP] && MaterialDraw(pos) == TRUE) {
        return 0;
    }

    pce = wP;
    for(pieceNum = 0; pieceNum < pos->pieceNum[wP]; ++pieceNum) {
        sq = pos->pieceList[pce][pieceNum];
        score += PawnTable[SQ64(sq)];

        if(IsolatedMask[SQ64(sq)] & pos->pawns[WHITE]) {
            score += PawnIsolated;
        }

        if( (WhitePassedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) {
            score += PawnPassed[Rank[sq]];
        }
    }

    pce = bP;
    for(pieceNum = 0; pieceNum < pos->pieceNum[bP]; ++pieceNum) {
        sq = pos->pieceList[pce][pieceNum];
        score -= PawnTable[MIRROR64(SQ64(sq))];

        if(IsolatedMask[SQ64(sq)] & pos->pawns[BLACK]) {
            score -= PawnIsolated;
        }

        if( (BlackPassedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0) {
            score -= PawnPassed[7 - Rank[sq]];
        }
    }

    pce = wN;
    for(pieceNum = 0; pieceNum < pos->pieceNum[wN]; ++pieceNum) {
        sq = pos->pieceList[pce][pieceNum];
        score += KnightTable[SQ64(sq)];
    }

    pce = bN;
    for(pieceNum = 0; pieceNum < pos->pieceNum[bN]; ++pieceNum) {
        sq = pos->pieceList[pce][pieceNum];
        score -= KnightTable[MIRROR64(SQ64(sq))];
    }

    pce = wB;
    for(pieceNum = 0; pieceNum < pos->pieceNum[wB]; ++pieceNum) {
        sq = pos->pieceList[pce][pieceNum];
        score += BishopTable[SQ64(sq)];
    }

    pce = bB;
    for(pieceNum = 0; pieceNum < pos->pieceNum[bB]; ++pieceNum) {
        sq = pos->pieceList[pce][pieceNum];
        score -= BishopTable[MIRROR64(SQ64(sq))];
    }

    pce = wR;
    for(pieceNum = 0; pieceNum < pos->pieceNum[wR]; ++pieceNum) {
        sq = pos->pieceList[pce][pieceNum];
        score += RookTable[SQ64(sq)];

        if(!(pos->pawns[BOTH] & FileBBMask[File[sq]])) {
            score += RookOpenFile;
        } else if(!(pos->pawns[WHITE] & FileBBMask[File[sq]])) {
            score += RookSemiOpenFile;
        }
    }

    pce = bR;
    for(pieceNum = 0; pieceNum < pos->pieceNum[bR]; ++pieceNum) {
        sq = pos->pieceList[pce][pieceNum];
        score -= RookTable[MIRROR64(SQ64(sq))];

        if(!(pos->pawns[BOTH] & FileBBMask[File[sq]])) {
            score -= RookOpenFile;
        } else if(!(pos->pawns[BLACK] & FileBBMask[File[sq]])) {
            score -= RookSemiOpenFile;
        }
    }

    pce = wQ;
    for(pieceNum = 0; pieceNum < pos->pieceNum[wQ]; ++pieceNum) {
        sq = pos->pieceList[pce][pieceNum];
        if(!(pos->pawns[BOTH] & FileBBMask[File[sq]])) {
            score += QueenOpenFile;
        } else if(!(pos->pawns[WHITE] & FileBBMask[File[sq]])) {
            score += QueenSemiOpenFile;
        }
    }

    pce = bQ;
    for(pieceNum = 0; pieceNum < pos->pieceNum[bQ]; ++pieceNum) {
        sq = pos->pieceList[pce][pieceNum];
        if(!(pos->pawns[BOTH] & FileBBMask[File[sq]])) {
            score -= QueenOpenFile;
        } else if(!(pos->pawns[BLACK] & FileBBMask[File[sq]])) {
            score -= QueenSemiOpenFile;
        }
    } 

    pce = wK;
	sq = pos->pieceList[pce][0];
	ASSERT(SquareOnBoard(sq));
	ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
	
	if( (pos->material[BLACK] <= ENDGAME_MAT) || (pos->pieceNum[bQ] == 0) ) {
		score += KingE[SQ64(sq)];
	} else {
		score += KingO[SQ64(sq)];
	}
	
	pce = bK;
	sq = pos->pieceList[pce][0];
	ASSERT(SquareOnBoard(sq));
	ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);
	
	if( (pos->material[WHITE] <= ENDGAME_MAT) || (pos->pieceNum[wQ] == 0) ) {
		score -= KingE[MIRROR64(SQ64(sq))];
	} else {
		score -= KingO[MIRROR64(SQ64(sq))];
	}

    return pos->side == WHITE ? score : -score;

    return 0;
}