#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sse.h"

#define FEN1 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 3"
#define FEN3 "8/3q4/8/8/8/4Qp2/8/8 w KQkq - 1 12"
#define FEN4 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 3"

#define WAC2 "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - -"
#define WAC1 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"


int main(int argc, char *argv[]) {

    Init();

    BOARD pos[1];
    SEARCHINFO info[1];
    info->quit = FALSE;
    pos->HashTable->pTable = NULL;
    InitHashTable(pos->HashTable, 64);

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    int ArgNum = 0;
    
    for(ArgNum = 0; ArgNum < argc; ++ArgNum) {
    	if(strncmp(argv[ArgNum], "NoBook", 6) == 0) {
    		EngineOptions->UseBook = FALSE;
    		printf("Book Off\n");
    	}
    }

    if(argc > 1) {
        if(!strncmp(argv[1], "xboard", 6)) {
            XBoard_Loop(pos, info);
            if(pos->HashTable->pTable != NULL) free(pos->HashTable->pTable);
            return 0;
        } else if(!strncmp(argv[1], "console", 7)) {
            Console_Loop(pos, info);
            if(pos->HashTable->pTable != NULL) free(pos->HashTable->pTable);
            return 0;
        } else if(!strncmp(argv[1], "uci", 3)) {
            Uci_Loop(pos, info);
            if(pos->HashTable->pTable != NULL) free(pos->HashTable->pTable);
            return 0;
        }
    }


    printf("Welcome to SSEH! Type 'uci' for UCI mode, 'xboard' for XBoard mode, 'console' for Console mode, or 'quit' to exit.\n"); 

    char line[256];
    while(TRUE) {
        memset(&line[0], 0, sizeof(line));
        fflush(stdout);

        if(!fgets(line, 256, stdin)) {
            continue;
        }

        if(line[0] == '\n') {
            continue;
        }

        if(!strncmp(line, "uci", 3)) {
            Uci_Loop(pos, info);
            if(pos->HashTable->pTable != NULL) free(pos->HashTable->pTable);
            return 0;
        } else if(!strncmp(line, "xboard", 6)) {
            XBoard_Loop(pos, info);
            if(pos->HashTable->pTable != NULL) free(pos->HashTable->pTable);
            return 0;
        } else if(!strncmp(line, "console", 7)) {
            Console_Loop(pos, info);
            if(pos->HashTable->pTable != NULL) free(pos->HashTable->pTable);
            return 0;
        } else if(!strncmp(line, "quit", 4)) {
            if(pos->HashTable->pTable != NULL) free(pos->HashTable->pTable);
            return 0;
        }
    }

    free(pos->HashTable->pTable);
    CleanPolyBook();

    return 0;
}