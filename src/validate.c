#include "sse.h"
#include <string.h>

int MoveListOk(const MOVE_LIST *list,  const BOARD *pos) {
    if(list->count < 0 || list->count >= MAX_POSITION_MOVES) {
        return FALSE;
    }

    int MoveNum;
    int from = 0;
    int to = 0;
    for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
        to = TOSQ(list->moves[MoveNum].move);
        from = FROMSQ(list->moves[MoveNum].move);
        if(!SquareOnBoard(to) || !SquareOnBoard(from)) {
            return FALSE;
        }
        if(!PieceValid(pos->pieces[from])) {
            PrintBoard(pos);
            return FALSE;
        }
    }

    return TRUE;
}

int SqIs120(const int sq) {
    return (sq>=0 && sq<120);
}

int PceValidEmptyOffbrd(const int pce) {
    return (PieceValidEmpty(pce) || pce == OFFBOARD);
}

int SquareOnBoard(const int sq) {
    return File[sq] == OFFBOARD ? 0 : 1;
}

int SideValid(const int side) {
    return side == WHITE || side == BLACK ? 1 : 0;
}

int FileRankValid(const int fr) {
    return fr >= 0 && fr <= 7 ? 1 : 0;
}

int PieceValidEmpty(const int pce) {
    return pce >= EMPTY && pce <= bK ? 1 : 0;
}

int PieceValid(const int pce) {
    return pce >= wP && pce <= bK ? 1 : 0;
}

void DebugAnalysisTest(BOARD *pos, SEARCHINFO *info) {

    FILE *file;
    file = fopen("lct2.epd","r");
    char lineIn [1024];

    info->depth = MAX_DEPTH;
    info->timeset = TRUE;
    int time = 1140000;


    if(file == NULL) {
        printf("File Not Found\n");
        return;
    }  else {
        while(fgets (lineIn , 1024 , file) != NULL) {
            info->starttime = GetTimeMs();
            info->stoptime = info->starttime + time;
            ClearHashTable(pos->HashTable);
            ParseFEN(lineIn, pos);
            printf("\n%s\n",lineIn);
            printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",
                time,info->starttime,info->stoptime,info->depth,info->timeset);
            SearchPosition(pos, info);
            memset(&lineIn[0], 0, sizeof(lineIn));
        }
    }
}

void MirrorEvalTest(BOARD *pos) {
    FILE *file;
    file = fopen("mirror.epd","r");
    char lineIn [1024];
    int ev1 = 0; int ev2 = 0;
    int positions = 0;
    if(file == NULL) {
        printf("File Not Found\n");
        return;
    }  else {
        while(fgets (lineIn , 1024 , file) != NULL) {
            ParseFEN(lineIn, pos);
            positions++;
            ev1 = EvalPosition(pos);
            MirrorBoard(pos);
            ev2 = EvalPosition(pos);

            if(ev1 != ev2) {
                printf("\n\n\n");
                ParseFEN(lineIn, pos);
                PrintBoard(pos);
                MirrorBoard(pos);
                PrintBoard(pos);
                printf("\n\nMirror Fail:\n%s\n",lineIn);
                getchar();
                return;
            }

            if( (positions % 1000) == 0)   {
                printf("position %d\n",positions);
            }

            memset(&lineIn[0], 0, sizeof(lineIn));
        }
    }
}