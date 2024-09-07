#include <stdio.h>
#include "sse.h"

int ParseFEN(char *fen, BOARD *pos) {

    ASSERT(fen != NULL);
    ASSERT(pos != NULL);

    int rank = RANK_8;
    int file = FILE_A;
    int piece = 0;
    int count = 0;
    int index = 0;
    int sq64 = 0;
    int sq120 = 0;

    ResetBoard(pos);

    while(rank >= RANK_1 && *fen) {
        count = 1;
        switch(*fen) {
            case 'p': piece = bP; break;
            case 'r': piece = bR; break;
            case 'n': piece = bN; break;
            case 'b': piece = bB; break;
            case 'k': piece = bK; break;
            case 'q': piece = bQ; break;
            case 'P': piece = wP; break;
            case 'R': piece = wR; break;
            case 'N': piece = wN; break;
            case 'B': piece = wB; break;
            case 'K': piece = wK; break;
            case 'Q': piece = wQ; break;

            case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': piece = EMPTY; count = *fen - '0'; break;

            case '/': case ' ': rank--; file = FILE_A; fen++; continue;

            default:
                printf("FEN error \n");
                return -1;
        }

        for(index = 0; index < count; ++index) {
            sq64 = FR2SQ(file, rank);
            sq120 = SQ120(sq64);
            if(piece != EMPTY) {
                pos->pieces[sq120] = piece;
            }
            file++;
        }
        fen++;
    }

    ASSERT(*fen == 'w' || *fen == 'b');

    pos->side = (*fen == 'w') ? WHITE : BLACK;
    fen += 2;

    for(index = 0; index < 4; ++index) {
        if(*fen == ' ') {
            break;
        }
        switch(*fen) {
            case 'K': pos->castlePerm |= WKCA; break;
            case 'Q': pos->castlePerm |= WQCA; break;
            case 'k': pos->castlePerm |= BKCA; break;
            case 'q': pos->castlePerm |= BQCA; break;
            default: break;
        }
        fen++;
    }
    fen++;

    ASSERT(pos->castlePerm >= 0 && pos->castlePerm <= 15);

    if(*fen != '-') {
        file = fen[0] - 'a';
        rank = fen[1] - '1';

        ASSERT(file >= FILE_A && file <= FILE_H);
        ASSERT(rank >= RANK_1 && rank <= RANK_8);

        pos->enPass = FR2SQ(file, rank);
    }

    pos->posKey = GeneratePosKey(pos);

    return 0;
}



void ResetBoard(BOARD *pos) {

    int index = 0;

    for(index = 0; index < BOARD_SIZE; ++index) {
        pos->pieces[index] = OFFBOARD;
    }

    for(index = 0; index < 64; ++index) {
        pos->pieces[SQ120(index)] = EMPTY;
    }

    for(index = 0; index < 3; ++index) {
        pos->bigPiece[index] = 0;
        pos->majorPiece[index] = 0;
        pos->minorPiece[index] = 0;
        pos->pawns[index] = 0ULL;
    }

    for(index = 0; index < 13; ++index) {
        pos->pieceNum[index] = 0;
    }

    for(index = 0; index < 2; ++index) {
        pos->KingSq[index] = NO_SQ;
    }

    pos->side = BOTH;
    pos->enPass = NO_SQ;
    pos->fiftyMove = 0;

    pos->ply = 0;
    pos->hisPly = 0;

    pos->castlePerm = 0;

    pos->posKey = 0ULL;

}