#include <stdio.h>
#include "sse.h"

long leafNodes;

void Perft(int depth, BOARD *pos) {
    
    ASSERT(CheckBoard(pos));

    if(depth == 0) {
        leafNodes++;
        return;
    }

    MOVE_LIST list[1];
    GenerateAllMoves(pos,list);

    int MoveNum = 0;
    for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {

        if(!MakeMove(pos,list->moves[MoveNum].move)) {
            continue;
        }
        Perft(depth - 1, pos);
        TakeMove(pos);
    }

    return;
}

void PerftTest(int depth, BOARD *pos) {
    
    ASSERT(CheckBoard(pos));

    PrintBoard(pos);
    printf("\nStarting Test To Depth:%d\n",depth);
    leafNodes = 0;
    
    MOVE_LIST list[1];
    GenerateAllMoves(pos,list);

    int Move;
    int MoveNum = 0;
    for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
        Move = list->moves[MoveNum].move;
        if(!MakeMove(pos,Move)) {
            continue;
        }
        long cumnodes = leafNodes;
        Perft(depth - 1, pos);
        TakeMove(pos);
        long oldnodes = leafNodes - cumnodes;
        printf("move %d : %s : %ld\n",MoveNum+1,PrintMove(Move),oldnodes);
    }
    
    printf("\nTest Complete : %ld nodes visited\n",leafNodes);
    
    return;
}