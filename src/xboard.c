#include "sse.h"
#include <stdio.h>
#include <string.h>

int ThreeFoldRep(const BOARD *pos) {

    ASSERT(CheckBoard(pos));

    int i = 0, r = 0;
    for(i = 0; i < pos->hisPly; ++i) {
        if(pos->history[i].posKey == pos->posKey) {
            r++;
        }
    }
    return r;
}

int DrawMaterial(const BOARD *pos) {

    ASSERT(CheckBoard(pos));

    if(pos->pieceNum[wP]!=0 || pos->pieceNum[bP]!=0) return FALSE;
    if(pos->pieceNum[wQ]!=0 || pos->pieceNum[bQ]!=0 || pos->pieceNum[wR]!=0 || pos->pieceNum[bR]!=0) return FALSE;
    if(pos->pieceNum[wB]>1 || pos->pieceNum[bB]>1) {return FALSE;}
    if(pos->pieceNum[wN]>1 || pos->pieceNum[bN]>1) {return FALSE;}
    if(pos->pieceNum[wN]!=0 && pos->pieceNum[wB]!=0) {return FALSE;}
    if(pos->pieceNum[bN]!=0 && pos->pieceNum[bB]!=0) {return FALSE;}

    return TRUE;
}

int CheckResult(BOARD *pos) {

    if(pos->fiftyMove >= 100) {
        printf("1/2-1/2 {fifty move rule (claimed by %s)}\n", NAME);
        return TRUE;
    }

    if(ThreeFoldRep(pos) >= 2) {
        printf("1/2-1/2 {3-fold repetition (claimed by %s)}\n", NAME);
        return TRUE;
    }

    if(DrawMaterial(pos) == TRUE) {
        printf("1/2-1/2 {insufficient material (claimed by %s)}\n", NAME);
        return TRUE;
    }

    MOVE_LIST list[1];
    GenerateAllMoves(pos, list);

    int MoveNum = 0;
    int found = 0;
    for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {

        if ( !MakeMove(pos, list->moves[MoveNum].move))  {
            continue;
        }
        found++;
        TakeMove(pos);
        break;
    }

    if(found != 0) return FALSE;

    int InCheck = SquareAttacked(pos->KingSq[pos->side], pos->side^1, pos);

    if(InCheck == TRUE) {
        if(pos->side == WHITE) {
            printf("0-1 {black mates (claimed by %s)}\n", NAME);
            return TRUE;
        } else {
            printf("0-1 {white mates (claimed by %s)}\n", NAME);
            return TRUE;
        }
    } else {
        printf("\n1/2-1/2 {stalemate (claimed by %s)}\n", NAME);
        return TRUE;
    }
    return FALSE;
}

void PrintOptions() {
    printf("feature ping=1 setboard=1 colors=0 usermove=1 memory=1\n");
    printf("feature done=1\n");
}

void XBoard_Loop(BOARD *pos, SEARCHINFO *info) {

    info->GAME_MODE = XBOARDMODE;
    info->POST_THINKING = TRUE;
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    PrintOptions();

    int depth = -1, movestogo[2] = {30,30 }, movetime = -1;
    int time = -1, inc = 0;
    int engineSide = BOTH;
    int timeLeft;
    int sec;
    int mps;
    int move = NOMOVE;
    char inBuf[80], command[80];
    int MB;

    engineSide = BLACK;
    ParseFEN(START_POSITION, pos);
    depth = -1;
    time = -1;

    while(TRUE) {

        fflush(stdout);

        if(pos->side == engineSide && CheckResult(pos) == FALSE) {
            info->starttime = GetTimeMs();
            info->depth = depth;

            if(time != -1) {
                info->timeset = TRUE;
                time /= movestogo[pos->side];
                time -= 50;
                info->stoptime = info->starttime + time + inc;
            }

            if(depth == -1 || depth > MAX_DEPTH) {
                info->depth = MAX_DEPTH;
            }

            printf("time:%d start:%d stop:%d depth:%d timeset:%d movestogo:%d mps:%d\n",
                time,info->starttime,info->stoptime,info->depth,info->timeset, movestogo[pos->side], mps);
                SearchPosition(pos, info);

            if(mps != 0) {
                movestogo[pos->side^1]--;
                if(movestogo[pos->side^1] < 1) {
                    movestogo[pos->side^1] = mps;
                }
            }

        }

        fflush(stdout);

        memset(&inBuf[0], 0, sizeof(inBuf));
        fflush(stdout);
        if (!fgets(inBuf, 80, stdin))
        continue;

        sscanf(inBuf, "%s", command);

        printf("command seen:%s\n",inBuf);

        if(!strcmp(command, "quit")) {
            info->quit = TRUE;
            break;
        }

        if(!strcmp(command, "force")) {
            engineSide = BOTH;
            continue;
        }

        if(!strcmp(command, "protover")){
            PrintOptions();
            continue;
        }

        if(!strcmp(command, "sd")) {
            sscanf(inBuf, "sd %d", &depth);
            printf("DEBUG depth:%d\n",depth);
            continue;
        }

        if(!strcmp(command, "st")) {
            sscanf(inBuf, "st %d", &movetime);
            printf("DEBUG movetime:%d\n",movetime);
            continue;
        }

        if(!strcmp(command, "time")) {
            sscanf(inBuf, "time %d", &time);
            time *= 10;
            printf("DEBUG time:%d\n",time);
            continue;
        }
        
        if(!strcmp(command, "memory")) {            
            sscanf(inBuf, "memory %d", &MB);        
            if(MB < 4) MB = 4;
            if(MB > MAX_HASH) MB = MAX_HASH;
            printf("Set Hash to %d MB\n",MB);
            InitHashTable(pos->HashTable, MB);
            continue;
        }

        if(!strcmp(command, "level")) {
            sec = 0;
            movetime = -1;
            if( sscanf(inBuf, "level %d %d %d", &mps, &timeLeft, &inc) != 3) {
              sscanf(inBuf, "level %d %d:%d %d", &mps, &timeLeft, &sec, &inc);
              printf("DEBUG level with :\n");
            }    else {
              printf("DEBUG level without :\n");
            }
            timeLeft *= 60000;
            timeLeft += sec * 1000;
            movestogo[0] = movestogo[1] = 30;
            if(mps != 0) {
                movestogo[0] = movestogo[1] = mps;
            }
            time = -1;
            printf("DEBUG level timeLeft:%d movesToGo:%d inc:%d mps%d\n",timeLeft,movestogo[0],inc,mps);
            continue;
        }

        if(!strcmp(command, "ping")) {
            printf("pong%s\n", inBuf+4);
            continue;
        }

        if(!strcmp(command, "new")) {
            engineSide = BLACK;
            ParseFEN(START_POSITION, pos);
            depth = -1;
            time = -1;
            continue;
        }

        if(!strcmp(command, "setboard")){
            engineSide = BOTH;
            ParseFEN(inBuf+9, pos);
            continue;
        }

        if(!strcmp(command, "go")) {
            engineSide = pos->side;
            continue;
        }

        if(!strcmp(command, "usermove")){
            movestogo[pos->side]--;
            move = ParseMove(inBuf+9, pos);
            if(move == NOMOVE) continue;
            MakeMove(pos, move);
            pos->ply=0;
        }
    }
}

void Console_Loop(BOARD *pos, SEARCHINFO *info) {

    printf("Welcome to SSEHC In Console Mode!\n");
    printf("Type help for commands\n\n");

    info->GAME_MODE = CONSOLEMODE;
    info->POST_THINKING = TRUE;
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    int depth = MAX_DEPTH, movetime = 3000;
    int engineSide = BOTH;
    int move = NOMOVE;
    char inBuf[80], command[80];

    engineSide = BLACK;
    ParseFEN(START_POSITION, pos);

    while(TRUE) {

        fflush(stdout);

        if(pos->side == engineSide && CheckResult(pos) == FALSE) {
            info->starttime = GetTimeMs();
            info->depth = depth;

            if(movetime != 0) {
                info->timeset = TRUE;
                info->stoptime = info->starttime + movetime;
            }

            SearchPosition(pos, info);
        }

        printf("\nSSEHC > ");

        fflush(stdout);

        memset(&inBuf[0], 0, sizeof(inBuf));
        fflush(stdout);
        if (!fgets(inBuf, 80, stdin))
        continue;

        sscanf(inBuf, "%s", command);

        if(!strcmp(command, "help")) {
            printf("Commands:\n");
            printf("quit - quit game\n");
            printf("force - computer will not think\n");
            printf("print - show board\n");
            printf("post - show thinking\n");
            printf("nopost - do not show thinking\n");
            printf("new - start new game\n");
            printf("go - set computer thinking\n");
            printf("depth x - set depth to x\n");
            printf("time x - set thinking time to x seconds (depth still applies if set)\n");
            printf("view - show current depth and movetime settings\n");
            printf("setboard x - set position to fen x\n");
            printf("** note ** - to reset time and depth, set to 0\n");
            printf("enter moves using b7b8q notation\n\n\n");
            continue;
        }

        if(!strcmp(command, "mirror")) {
            engineSide = BOTH;
            MirrorEvalTest(pos);
            continue;
        }

        if(!strcmp(command, "eval")) {
            PrintBoard(pos);
            printf("Eval:%d",EvalPosition(pos));
            MirrorBoard(pos);
            PrintBoard(pos);
            printf("Eval:%d",EvalPosition(pos));
            continue;
        }

        if(!strcmp(command, "setboard")){
            engineSide = BOTH;
            ParseFEN(inBuf+9, pos);
            continue;
        }

        if(!strcmp(command, "quit")) {
            info->quit = TRUE;
            break;
        }

        if(!strcmp(command, "post")) {
            info->POST_THINKING = TRUE;
            continue;
        }

        if(!strcmp(command, "print")) {
            PrintBoard(pos);
            continue;
        }

        if(!strcmp(command, "nopost")) {
            info->POST_THINKING = FALSE;
            continue;
        }

        if(!strcmp(command, "force")) {
            engineSide = BOTH;
            continue;
        }

        if(!strcmp(command, "view")) {
            if(depth == MAX_DEPTH) printf("depth not set ");
            else printf("depth %d",depth);

            if(movetime != 0) printf(" movetime %ds\n",movetime/1000);
            else printf(" movetime not set\n");

            continue;
        }

        if(!strcmp(command, "depth")) {
            sscanf(inBuf, "depth %d", &depth);
            if(depth==0) depth = MAX_DEPTH;
            continue;
        }

        if(!strcmp(command, "time")) {
            sscanf(inBuf, "time %d", &movetime);
            movetime *= 1000;
            continue;
        }

        if(!strcmp(command, "new")) {
            engineSide = BLACK;
            ParseFEN(START_POSITION, pos);
            continue;
        }

        if(!strcmp(command, "go")) {
            engineSide = pos->side;
            continue;
        }

        move = ParseMove(inBuf, pos);
        if(move == NOMOVE) {
            printf("Command unknown:%s\n",inBuf);
            continue;
        }
        MakeMove(pos, move);
        pos->ply=0;
    }
}
