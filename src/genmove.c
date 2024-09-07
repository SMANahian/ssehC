#include <stdio.h>
#include "sse.h"

#define MOVE_INT(f, t, ca, pro, fl) ( (f) | ( (t) << 7 ) | ( (ca) << 14 ) | ( (pro) << 20 ) | (fl) )
#define SQOFFBOARD(sq) (File[sq]==OFFBOARD) 



int LoopSlidePiece[8] = { wB, wR, wQ, 0, bB, bR, bQ, 0 };
int LoopNonSlidePiece[6] = { wN, wK, 0, bN, bK, 0 };

int LoopSlideIndex[2] = { 0, 4 };
int LoopNonSlideIndex[2] = { 0, 3 };


void AddNormalMove(const BOARD *pos, int move, MOVE_LIST *list) {
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++; 
}

void AddCaptureMove(const BOARD *pos, int move, MOVE_LIST *list) {
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++; 
}

void AddEnPassantMove(const BOARD *pos, int move, MOVE_LIST *list) {
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++; 
}

void AddWhitePawnCapMove(const BOARD *pos, const int from, const int to, const int cap, MOVE_LIST *list) {

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

void AddWhitePawnMove(const BOARD *pos, const int from, const int to, MOVE_LIST *list) {

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

void AddBlackPawnCapMove(const BOARD *pos, const int from, const int to, const int cap, MOVE_LIST *list) {

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

void AddBlackPawnMove(const BOARD *pos, const int from, const int to, MOVE_LIST *list) {

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
    }

    

}