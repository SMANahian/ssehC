#include <stdio.h>
#include "sse.h"

int GetPvLine(const int depth, BOARD *pos) {
    ASSERT(depth < MAX_DEPTH && depth >= 1);

    int move = ProbePvTable(pos);
    int count = 0;

    while(move != NOMOVE && count < depth) {
        ASSERT(count < MAX_DEPTH);
        if(MoveExists(pos, move)) {
            MakeMove(pos, move);
            pos->PvArray[count++] = move;
        } else {
            break;
        }
        move = ProbePvTable(pos);
    }

    while(pos->ply > 0) {
        TakeMove(pos);
    }

    return count;
}

const int PvSize = 0x100000 * 2;

void ClearPvTable(PV_TABLE *table) {
    PV_ENTRY *pvEntry;
    for(pvEntry = table->pTable; pvEntry < table->pTable + table->numEntries; pvEntry++) {
        pvEntry->posKey = 0ULL;
        pvEntry->move = NOMOVE;
    }
}

void InitPvTable(PV_TABLE *table) {
    table->numEntries = PvSize / sizeof(PV_ENTRY);
    table->numEntries -= 2;
    free(table->pTable);
    table->pTable = (PV_ENTRY *)malloc(table->numEntries * sizeof(PV_ENTRY));
    ClearPvTable(table);
}

void StorePvMove(const BOARD *pos, const int move) {
    int index = pos->posKey % pos->PvTable->numEntries;
    ASSERT(index >= 0 && index <= pos->PvTable->numEntries - 1);
    pos->PvTable->pTable[index].move = move;
    pos->PvTable->pTable[index].posKey = pos->posKey;
}

int ProbePvTable(const BOARD *pos) {
    int index = pos->posKey % pos->PvTable->numEntries;
    ASSERT(index >= 0 && index <= pos->PvTable->numEntries - 1);
    if(pos->PvTable->pTable[index].posKey == pos->posKey) {
        return pos->PvTable->pTable[index].move;
    }
    return NOMOVE;
}