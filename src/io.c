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