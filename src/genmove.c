#include <stdio.h>
#include "sse.h"

#define MOVE_INT(f, t, ca, pro, fl) ( (f) | ( (t) << 7 ) | ( (ca) << 14 ) | ( (pro) << 20 ) | (fl) )
#define SQOFFBOARD(sq) (File[sq]==OFFBOARD) 



const int LoopSlidePiece[8] = { wB, wR, wQ, 0, bB, bR, bQ, 0 };
const int LoopNonSlidePiece[6] = { wN, wK, 0, bN, bK, 0 };

const int LoopSlideIndex[2] = { 0, 4 };
const int LoopNonSlideIndex[2] = { 0, 3 };

const int PieceDir[13][8] = {
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { -8, -19, -21, -12, 8, 19, 21, 12 },
    { -9, -11, 11, 9, 0, 0, 0, 0 },
    { -1, -10, 1, 10, 0, 0, 0, 0 },
    { -1, -10, 1, 10, -9, -11, 11, 9 },
    { -1, -10, 1, 10, -9, -11, 11, 9 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { -8, -19, -21, -12, 8, 19, 21, 12 },
    { -9, -11, 11, 9, 0, 0, 0, 0 },
    { -1, -10, 1, 10, 0, 0, 0, 0 },
    { -1, -10, 1, 10, -9, -11, 11, 9 },
    { -1, -10, 1, 10, -9, -11, 11, 9 }
};

const int NumDir[13] = { 0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8 };

const int VictimScore[13] = { 0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600 };
static int MvvLvaScores[13][13];

int InitMvvLva() {
    int Attacker;
    int Victim;

    for(Attacker = wP; Attacker <= bK; ++Attacker) {
        for(Victim = wP; Victim <= bK; ++Victim) {
            MvvLvaScores[Victim][Attacker] = VictimScore[Victim] + 6 - (VictimScore[Attacker] / 100);
        }
    }
    
    return 0;
}

int MoveExists(BOARD *pos, const int move) {

    MOVE_LIST list[1];
    GenerateAllMoves(pos, list);

    int moveNum = 0;
    for(moveNum = 0; moveNum < list->count; ++moveNum) {
        if(!MakeMove(pos, list->moves[moveNum].move)) {
            continue;
        }
        TakeMove(pos);
        if(list->moves[moveNum].move == move) {
            return TRUE;
        }
    }
    return FALSE;
} 

static void AddNormalMove(const BOARD *pos, int move, MOVE_LIST *list) {

    ASSERT(SquareOnBoard(FROMSQ(move)));
    ASSERT(SquareOnBoard(TOSQ(move)));

    list->moves[list->count].move = move;

    if(pos->searchKillers[0][pos->ply] == move) {
        list->moves[list->count].score = 900000;
    } else if(pos->searchKillers[1][pos->ply] == move) {
        list->moves[list->count].score = 800000;
    } else {
        list->moves[list->count].score = pos->searchHistory[pos->pieces[FROMSQ(move)]][TOSQ(move)];
    }

    list->count++; 
}

static void AddCaptureMove(const BOARD *pos, int move, MOVE_LIST *list) {

    ASSERT(SquareOnBoard(FROMSQ(move)));
    ASSERT(SquareOnBoard(TOSQ(move)));
    ASSERT(PieceValid(CAPTURED(move)));

    list->moves[list->count].move = move;
    list->moves[list->count].score = MvvLvaScores[CAPTURED(move)][pos->pieces[FROMSQ(move)]] + 1000000; 
    list->count++; 
}

static void AddEnPassantMove(const BOARD *pos, int move, MOVE_LIST *list) {

    ASSERT(SquareOnBoard(FROMSQ(move)));
    ASSERT(SquareOnBoard(TOSQ(move)));

    list->moves[list->count].move = move;
    list->moves[list->count].score = 105 + 1000000;
    list->count++; 
}

static void AddWhitePawnCapMove(const BOARD *pos, const int from, const int to, const int cap, MOVE_LIST *list) {

    ASSERT(PieceValidEmpty(cap));
    ASSERT(SquareOnBoard(from));
    ASSERT(SquareOnBoard(to));

    if(Rank[from] == RANK_7) {
        AddCaptureMove(pos, MOVE_INT(from, to, cap, wQ, 0), list);
        AddCaptureMove(pos, MOVE_INT(from, to, cap, wR, 0), list);
        AddCaptureMove(pos, MOVE_INT(from, to, cap, wB, 0), list);
        AddCaptureMove(pos, MOVE_INT(from, to, cap, wN, 0), list);
    } else {
        AddCaptureMove(pos, MOVE_INT(from, to, cap, EMPTY, 0), list);
    }
}

static void AddWhitePawnMove(const BOARD *pos, const int from, const int to, MOVE_LIST *list) {

    ASSERT(SquareOnBoard(from));
    ASSERT(SquareOnBoard(to));

    
    if(Rank[from] == RANK_7) {
        AddNormalMove(pos, MOVE_INT(from, to, EMPTY, wQ, 0), list);
        AddNormalMove(pos, MOVE_INT(from, to, EMPTY, wR, 0), list);
        AddNormalMove(pos, MOVE_INT(from, to, EMPTY, wB, 0), list);
        AddNormalMove(pos, MOVE_INT(from, to, EMPTY, wN, 0), list);
    } else {
        AddNormalMove(pos, MOVE_INT(from, to, EMPTY, EMPTY, 0), list);
    }
}

static void AddBlackPawnCapMove(const BOARD *pos, const int from, const int to, const int cap, MOVE_LIST *list) {

    ASSERT(PieceValidEmpty(cap));
    ASSERT(SquareOnBoard(from));
    ASSERT(SquareOnBoard(to));

    
    if(Rank[from] == RANK_2) {
        AddCaptureMove(pos, MOVE_INT(from, to, cap, bQ, 0), list);
        AddCaptureMove(pos, MOVE_INT(from, to, cap, bR, 0), list);
        AddCaptureMove(pos, MOVE_INT(from, to, cap, bB, 0), list);
        AddCaptureMove(pos, MOVE_INT(from, to, cap, bN, 0), list);
    } else {
        AddCaptureMove(pos, MOVE_INT(from, to, cap, EMPTY, 0), list);
    }
}

static void AddBlackPawnMove(const BOARD *pos, const int from, const int to, MOVE_LIST *list) {

    ASSERT(SquareOnBoard(from));
    ASSERT(SquareOnBoard(to));

    
    if(Rank[from] == RANK_2) {
        AddNormalMove(pos, MOVE_INT(from, to, EMPTY, bQ, 0), list);
        AddNormalMove(pos, MOVE_INT(from, to, EMPTY, bR, 0), list);
        AddNormalMove(pos, MOVE_INT(from, to, EMPTY, bB, 0), list);
        AddNormalMove(pos, MOVE_INT(from, to, EMPTY, bN, 0), list);
    } else {
        AddNormalMove(pos, MOVE_INT(from, to, EMPTY, EMPTY, 0), list);
    }
}

void GenerateAllMoves(const BOARD *pos, MOVE_LIST *list) {

    ASSERT(CheckBoard(pos));

    list->count = 0;

    int pce = EMPTY;
    int side = pos->side;
    int sq = 0; int t_sq = 0;
    int pceNum = 0;
    int dir = 0;
    int index = 0;
    int pceIndex = 0;
    int pceNumIndex = 0;
    
    if(side == WHITE) {
        for(pceNum = 0; pceNum < pos->pieceNum[wP]; ++pceNum) {
            sq = pos->pieceList[wP][pceNum];
            ASSERT(SquareOnBoard(sq));
            if(pos->pieces[sq + 10] == EMPTY) {
                AddWhitePawnMove(pos, sq, sq + 10, list);
                if(Rank[sq] == RANK_2 && pos->pieces[sq + 20] == EMPTY) {
                    AddNormalMove(pos, MOVE_INT(sq, (sq + 20), EMPTY, EMPTY, MASK_PS), list);
                }
            }
            if(!SQOFFBOARD(sq + 9) && PieceColor[pos->pieces[sq + 9]] == BLACK) {
                AddWhitePawnCapMove(pos, sq, sq + 9, pos->pieces[sq + 9], list);
            }
            if(!SQOFFBOARD(sq + 11) && PieceColor[pos->pieces[sq  + 11]] == BLACK) {
                AddWhitePawnCapMove(pos, sq, sq + 11, pos->pieces[sq + 11], list);
            }
            if(pos->enPass != NO_SQ) {
                if(sq + 9 == pos->enPass) {
                    AddEnPassantMove(pos, MOVE_INT(sq, sq + 9, EMPTY, EMPTY, MASK_EP), list);
                }
                if(sq + 11 == pos->enPass) {
                    AddEnPassantMove(pos, MOVE_INT(sq, sq + 11, EMPTY, EMPTY, MASK_EP), list);
                }
            }
        } 

        // castling
        if(pos->castlePerm & WKCA) {
            if(pos->pieces[F1] == EMPTY && pos->pieces[G1] == EMPTY) {
                if(!SquareAttacked(E1, BLACK, pos) && !SquareAttacked(F1, BLACK, pos) && !SquareAttacked(G1, BLACK, pos)) {
                    AddNormalMove(pos, MOVE_INT(E1, G1, EMPTY, EMPTY, MASK_CA), list);
                }
            }
        }

        if(pos->castlePerm & WQCA) {
            if(pos->pieces[D1] == EMPTY && pos->pieces[C1] == EMPTY && pos->pieces[B1] == EMPTY) {
                if(!SquareAttacked(E1, BLACK, pos) && !SquareAttacked(D1, BLACK, pos) && !SquareAttacked(C1, BLACK, pos)) {
                    AddNormalMove(pos, MOVE_INT(E1, C1, EMPTY, EMPTY, MASK_CA), list);
                }
            }
        }

    } else {
        for(pceNum = 0; pceNum < pos->pieceNum[bP]; ++pceNum) {
            sq = pos->pieceList[bP][pceNum];
            ASSERT(SquareOnBoard(sq));
            if(pos->pieces[sq - 10] == EMPTY) {
                AddBlackPawnMove(pos, sq, sq - 10, list);
                if(Rank[sq] == RANK_7 && pos->pieces[sq - 20] == EMPTY) {
                    AddNormalMove(pos, MOVE_INT(sq, (sq - 20), EMPTY, EMPTY, MASK_PS), list);
                }
            }
            if(!SQOFFBOARD(sq - 9) && PieceColor[pos->pieces[sq - 9]] == WHITE) {
                AddBlackPawnCapMove(pos, sq, sq - 9, pos->pieces[sq - 9], list);
            }
            if(!SQOFFBOARD(sq - 11) && PieceColor[pos->pieces[sq - 11]] == WHITE) {
                AddBlackPawnCapMove(pos, sq, sq - 11, pos->pieces[sq - 11], list);
            }
            if(pos->enPass != NO_SQ) {
                if(sq - 9 == pos->enPass) {
                    AddEnPassantMove(pos, MOVE_INT(sq, sq - 9, EMPTY, EMPTY, MASK_EP), list);
                }
                if(sq - 11 == pos->enPass) {
                    AddEnPassantMove(pos, MOVE_INT(sq, sq - 11, EMPTY, EMPTY, MASK_EP), list);
                }
            }
        }

        // castling
        if(pos->castlePerm & BKCA) {
            if(pos->pieces[F8] == EMPTY && pos->pieces[G8] == EMPTY) {
                if(!SquareAttacked(E8, WHITE, pos) && !SquareAttacked(F8, WHITE, pos) && !SquareAttacked(G8, WHITE, pos)) {
                    AddNormalMove(pos, MOVE_INT(E8, G8, EMPTY, EMPTY, MASK_CA), list);
                }
            }
        }

        if(pos->castlePerm & BQCA) {
            if(pos->pieces[D8] == EMPTY && pos->pieces[C8] == EMPTY && pos->pieces[B8] == EMPTY) {
                if(!SquareAttacked(E8, WHITE, pos) && !SquareAttacked(D8, WHITE, pos) && !SquareAttacked(C8, WHITE, pos)) {
                    AddNormalMove(pos, MOVE_INT(E8, C8, EMPTY, EMPTY, MASK_CA), list);
                }
            }
        }

    }


    // Sliding pieces
    pceIndex = LoopSlideIndex[side];
    pce = LoopSlidePiece[pceIndex++];

    while(pce != 0) {
        ASSERT(PieceValid(pce));

        for(pceNum = 0; pceNum < pos->pieceNum[pce]; ++pceNum) {
            sq = pos->pieceList[pce][pceNum];
            ASSERT(SquareOnBoard(sq));

            for(index = 0; index < NumDir[pce]; ++index) {
                dir = PieceDir[pce][index];
                t_sq = sq + dir;

                while(!SQOFFBOARD(t_sq)) {
                    if(pos->pieces[t_sq] != EMPTY) {
                        if(PieceColor[pos->pieces[t_sq]] != side) {
                            AddCaptureMove(pos, MOVE_INT(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
                        }
                        break;
                    }
                    AddNormalMove(pos, MOVE_INT(sq, t_sq, EMPTY, EMPTY, 0), list);
                    t_sq += dir;
                }
            }

        }
        
        pce = LoopSlidePiece[pceIndex++];
    }

    // Non-sliding pieces
    pceIndex = LoopNonSlideIndex[side];
    pce = LoopNonSlidePiece[pceIndex++];

    while(pce != 0) {
        ASSERT(PieceValid(pce));

        for(pceNum = 0; pceNum < pos->pieceNum[pce]; ++pceNum) {
            sq = pos->pieceList[pce][pceNum];
            ASSERT(SquareOnBoard(sq));

            for(index = 0; index < NumDir[pce]; ++index) {
                dir = PieceDir[pce][index];
                t_sq = sq + dir;

                if(SQOFFBOARD(t_sq)) {
                    continue;
                }

                if(pos->pieces[t_sq] != EMPTY) {
                    if(PieceColor[pos->pieces[t_sq]] != side) {
                        AddCaptureMove(pos, MOVE_INT(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
                    }
                    continue;
                }
                AddNormalMove(pos, MOVE_INT(sq, t_sq, EMPTY, EMPTY, 0), list);
            }

        }
        
        pce = LoopNonSlidePiece[pceIndex++];
    }
}

void GenerateAllCaptures(const BOARD *pos, MOVE_LIST *list) {

    ASSERT(CheckBoard(pos));

    list->count = 0;

    int pce = EMPTY;
    int side = pos->side;
    int sq = 0; int t_sq = 0;
    int pceNum = 0;
    int dir = 0;
    int index = 0;
    int pceIndex = 0;
    int pceNumIndex = 0;
    
    if(side == WHITE) {
        for(pceNum = 0; pceNum < pos->pieceNum[wP]; ++pceNum) {
            sq = pos->pieceList[wP][pceNum];
            ASSERT(SquareOnBoard(sq));
            if(!SQOFFBOARD(sq + 9) && PieceColor[pos->pieces[sq + 9]] == BLACK) {
                AddWhitePawnCapMove(pos, sq, sq + 9, pos->pieces[sq + 9], list);
            }
            if(!SQOFFBOARD(sq + 11) && PieceColor[pos->pieces[sq  + 11]] == BLACK) {
                AddWhitePawnCapMove(pos, sq, sq + 11, pos->pieces[sq + 11], list);
            }
            if(pos->enPass != NO_SQ) {
                if(sq + 9 == pos->enPass) {
                    AddEnPassantMove(pos, MOVE_INT(sq, sq + 9, EMPTY, EMPTY, MASK_EP), list);
                }
                if(sq + 11 == pos->enPass) {
                    AddEnPassantMove(pos, MOVE_INT(sq, sq + 11, EMPTY, EMPTY, MASK_EP), list);
                }
            }
        }
    } else {
        for(pceNum = 0; pceNum < pos->pieceNum[bP]; ++pceNum) {
            sq = pos->pieceList[bP][pceNum];
            ASSERT(SquareOnBoard(sq));
            if(!SQOFFBOARD(sq - 9) && PieceColor[pos->pieces[sq - 9]] == WHITE) {
                AddBlackPawnCapMove(pos, sq, sq - 9, pos->pieces[sq - 9], list);
            }
            if(!SQOFFBOARD(sq - 11) && PieceColor[pos->pieces[sq - 11]] == WHITE) {
                AddBlackPawnCapMove(pos, sq, sq - 11, pos->pieces[sq - 11], list);
            }
            if(pos->enPass != NO_SQ) {
                if(sq - 9 == pos->enPass) {
                    AddEnPassantMove(pos, MOVE_INT(sq, sq - 9, EMPTY, EMPTY, MASK_EP), list);
                }
                if(sq - 11 == pos->enPass) {
                    AddEnPassantMove(pos, MOVE_INT(sq, sq - 11, EMPTY, EMPTY, MASK_EP), list);
                }
            }
        }
    }


    // Sliding pieces
    pceIndex = LoopSlideIndex[side];
    pce = LoopSlidePiece[pceIndex++];

    while(pce != 0) {
        ASSERT(PieceValid(pce));

        for(pceNum = 0; pceNum < pos->pieceNum[pce]; ++pceNum) {
            sq = pos->pieceList[pce][pceNum];
            ASSERT(SquareOnBoard(sq));

            for(index = 0; index < NumDir[pce]; ++index) {
                dir = PieceDir[pce][index];
                t_sq = sq + dir;

                while(!SQOFFBOARD(t_sq)) {
                    if(pos->pieces[t_sq] != EMPTY) {
                        if(PieceColor[pos->pieces[t_sq]] != side) {
                            AddCaptureMove(pos, MOVE_INT(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
                        }
                        break;
                    }
                    t_sq += dir;
                }
            }

        }
        
        pce = LoopSlidePiece[pceIndex++];
    }

    // Non-sliding pieces
    pceIndex = LoopNonSlideIndex[side];
    pce = LoopNonSlidePiece[pceIndex++];

    while(pce != 0) {
        ASSERT(PieceValid(pce));

        for(pceNum = 0; pceNum < pos->pieceNum[pce]; ++pceNum) {
            sq = pos->pieceList[pce][pceNum];
            ASSERT(SquareOnBoard(sq));

            for(index = 0; index < NumDir[pce]; ++index) {
                dir = PieceDir[pce][index];
                t_sq = sq + dir;

                if(SQOFFBOARD(t_sq)) {
                    continue;
                }

                if(pos->pieces[t_sq] != EMPTY) {
                    if(PieceColor[pos->pieces[t_sq]] != side) {
                        AddCaptureMove(pos, MOVE_INT(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
                    }
                    continue;
                }
            }

        }
        
        pce = LoopNonSlidePiece[pceIndex++];
    }
}