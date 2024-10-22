#include <stdio.h>
#include "sse.h"

int IsRepetition(const BOARD *pos) {

    int index = 0;

    for(index = pos->hisPly - pos->fiftyMove; index < pos->hisPly - 1; ++index) {
        ASSERT(index >= 0 && index < MAX_GAME_MOVES);
        if(pos->posKey == pos->history[index].posKey) {
            return TRUE;
        }
    }

    return FALSE;
}

void SearchPosition(BOARD *pos) {
    
} 

