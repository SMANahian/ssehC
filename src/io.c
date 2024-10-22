#include <stdio.h>
#include "sse.h"

char *SquareString(const int sq) {
    static char sqStr[3];

    int file = File[sq];
    int rank = Rank[sq];

    sprintf(sqStr, "%c%c", ('a' + file), ('1' + rank));

    return sqStr;
}


char *PrintMove(const int move) {
    static char MvStr[6];

    int ff = File[FROMSQ(move)];
    int rf = Rank[FROMSQ(move)];
    int ft = File[TOSQ(move)];
    int rt = Rank[TOSQ(move)];

    int promoted = PROMOTED(move);

    if(promoted) {
        char pchar = 'q';
        if(IsN(promoted)) {
            pchar = 'n';
        } else if(IsRQ(promoted) && !IsBQ(promoted))  {
            pchar = 'r';
        } else if(!IsRQ(promoted) && IsBQ(promoted))   {
            pchar = 'b';
        }
        sprintf(MvStr, "%c%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt), pchar);
    } else {
        sprintf(MvStr, "%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt));
    }

    return MvStr;
}

int ParseMove(char *ptrChar, BOARD *pos) {
    if(ptrChar[1] > '8' || ptrChar[1] < '1') return NOMOVE;
    if(ptrChar[3] > '8' || ptrChar[3] < '1') return NOMOVE;
    if(ptrChar[0] > 'h' || ptrChar[0] < 'a') return NOMOVE;
    if(ptrChar[2] > 'h' || ptrChar[2] < 'a') return NOMOVE;

    int from = FR2SQ(ptrChar[0] - 'a', ptrChar[1] - '1');
    int to = FR2SQ(ptrChar[2] - 'a', ptrChar[3] - '1');

    ASSERT(SquareOnBoard(from) && SquareOnBoard(to));

    MOVE_LIST list[1];
    GenerateAllMoves(pos, list);

    int MoveNum = 0;
    int Move = 0;
    int PromPce = EMPTY;

    for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
        Move = list->moves[MoveNum].move;
        if(FROMSQ(Move) == from && TOSQ(Move) == to) {
            PromPce = PROMOTED(Move);
            if(PromPce != EMPTY) {
                if(IsRQ(PromPce) && !IsBQ(PromPce) && ptrChar[4] == 'r') {
                    return Move;
                } else if(!IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4] == 'b') {
                    return Move;
                } else if(IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4] == 'q') {
                    return Move;
                } else if(IsN(PromPce) && ptrChar[4] == 'n') {
                    return Move;
                }
                continue;
            }
            return Move;
        }
    }

    return NOMOVE;
}

void PrintMoveList(const MOVE_LIST *list) {
    int index = 0;
    int score = 0;
    printf("MoveList:\n");

    for(index = 0; index < list->count; ++index) {
        score = list->moves[index].score;
        printf("Move:%d > %s (score:%d)\n",index+1,PrintMove(list->moves[index].move),score);
    }
    printf("MoveList Total %d Moves:\n\n",list->count);
} 