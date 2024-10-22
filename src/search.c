#include <stdio.h>
#include "sse.h"

static void CheckUp() {
    
}

static void PickNextMove(int moveNum, MOVE_LIST *list) {
    MOVE temp;
    int bestScore = 0;
    int bestNum = moveNum;

    for(int index = moveNum; index < list->count; ++index) {
        if(list->moves[index].score > bestScore) {
            bestScore = list->moves[index].score;
            bestNum = index;
        }
    }

    ASSERT(moveNum >= 0 && moveNum < list->count);
    ASSERT(bestNum >= 0 && bestNum < list->count);

    ASSERT(bestNum >= moveNum);
    temp = list->moves[moveNum];
    list->moves[moveNum] = list->moves[bestNum];
    list->moves[bestNum] = temp;
}

static int IsRepetition(const BOARD *pos) {

    int index = 0;

    for(index = pos->hisPly - pos->fiftyMove; index < pos->hisPly - 1; ++index) {
        ASSERT(index >= 0 && index < MAX_GAME_MOVES);
        if(pos->posKey == pos->history[index].posKey) {
            return TRUE;
        }
    }

    return FALSE;
}

static void ClearForSearch(BOARD *pos, SEARCHINFO *info) {

    int index = 0;
    int index2 = 0;

    for(index = 0; index < 13; ++index) {
        for(index2 = 0; index2 < BOARD_SIZE; ++index2) {
            pos->searchHistory[index][index2] = 0;
        }
    }

    for(index = 0; index < 2; ++index) {
        for(index2 = 0; index2 < MAX_DEPTH; ++index2) {
            pos->searchKillers[index][index2] = 0;
        }
    }

    ClearPvTable(pos->PvTable);
    pos->ply = 0;

    info->starttime = GetTimeMs();
    info->stopped = 0;
    info->nodes = 0;
    info->fh = 0;
    info->fhf = 0;

}

static int Quiessence(int alpha, int beta, BOARD *pos, SEARCHINFO *info) {
    return 0;
}

static int AlphaBeta(int alpha, int beta, int depth, BOARD *pos, SEARCHINFO *info, int DoNull) {
    
    ASSERT(CheckBoard(pos));

    if(depth <= 0) {
        info->nodes++;
        return EvalPosition(pos);
    }

    info->nodes++;

    if((IsRepetition(pos) || pos->fiftyMove >= 100) && pos->ply) {
        return 0;
    }

    if(pos->ply > MAX_DEPTH - 1) {
        return EvalPosition(pos);
    }

    MOVE_LIST list[1];
    GenerateAllMoves(pos, list);

    int MoveNum = 0;
    int Legal = 0;
    int OldAlpha = alpha;
    int BestMove = NOMOVE;
    int Score = -INFINITE;
    int PvMove = ProbePvTable(pos);

    if(PvMove != NOMOVE) {
        for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
            if(list->moves[MoveNum].move == PvMove) {
                list->moves[MoveNum].score = 2000000;
                break;
            }
        }
    }

    for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {

        PickNextMove(MoveNum, list);

        if(!MakeMove(pos, list->moves[MoveNum].move)) {
            continue;
        }

        Legal++;
        Score = -AlphaBeta(-beta, -alpha, depth - 1, pos, info, TRUE);
        TakeMove(pos);

        if(Score > alpha) {
            if(Score >= beta) {
                if(Legal == 1) {
                    info->fhf++;
                }
                info->fh++;

                if(!(list->moves[MoveNum].move & MASK_CAP)) {
                    pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
                    pos->searchKillers[0][pos->ply] = list->moves[MoveNum].move;
                } 

                return beta;
            }
            alpha = Score;
            BestMove = list->moves[MoveNum].move;

            if(!(list->moves[MoveNum].move & MASK_CAP)) {
                pos->searchHistory[pos->pieces[FROMSQ(BestMove)]][TOSQ(BestMove)] += depth;
            }
        }
    }

    if(Legal == 0) {
        if(SquareAttacked(pos->KingSq[pos->side], pos->side ^ 1, pos) == TRUE) {
            return -INFINITE + pos->ply;
        } else {
            return 0;
        }
    }

    if(alpha != OldAlpha) {
        StorePvMove(pos, BestMove);
    }

    return alpha;
}

void SearchPosition(BOARD *pos, SEARCHINFO *info ) {

    int bestMove = NOMOVE;
    int bestScore = -INFINITE;
    int currentDepth = 0;
    int pvMoves = 0;
    int pvNum = 0;

    ClearForSearch(pos, info);

    for(currentDepth = 1; currentDepth <= info->depth; ++currentDepth) {

        bestScore = AlphaBeta(-INFINITE, INFINITE, currentDepth, pos, info, TRUE);

        if(info->stopped == TRUE) {
            break;
        }

        pvMoves = GetPvLine(currentDepth, pos);
        bestMove = pos->PvArray[0];

        printf("Depth:%d Score:%d BestMove:%s Nodes:%ld\n", currentDepth, bestScore, PrintMove(bestMove), info->nodes);

        printf("pv");
        for(pvNum = 0; pvNum < pvMoves; ++pvNum) {
            printf(" %s", PrintMove(pos->PvArray[pvNum]));
        }
        printf("\n");
        printf("Ordering:%.2f\n", (info->fhf/info->fh));

    } 
    
} 

