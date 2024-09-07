#include <stdio.h>
#include "sse.h"


int CheckBoard(const BOARD *pos) {

    int t_pieceNum[13] = {0};
    int t_bigPiece[2] = {0};
    int t_majorPiece[2] = {0};
    int t_minorPiece[2] = {0};
    int t_material[2] = {0};

    int sq64, t_piece, t_pceNum, sq120, color, pcount;

    U64 t_pawns[3] = {0ULL};

    t_pawns[WHITE] = pos->pawns[WHITE];
    t_pawns[BLACK] = pos->pawns[BLACK];
    t_pawns[BOTH] = pos->pawns[BOTH];

    for(t_piece = wP; t_piece <= bK; ++t_piece) {
        for(t_pceNum = 0; t_pceNum < pos->pieceNum[t_piece]; ++t_pceNum) {
            sq120 = pos->pieceList[t_piece][t_pceNum];
            ASSERT(pos->pieces[sq120] == t_piece);
        }
    }

    for(sq64 = 0; sq64 < 64; ++sq64) {
        sq120 = SQ120(sq64);
        t_piece = pos->pieces[sq120];
        t_pieceNum[t_piece]++;
        color = PieceColor[t_piece];
        if(PieceBig[t_piece] == TRUE) t_bigPiece[color]++;
        if(PieceMajor[t_piece] == TRUE) t_majorPiece[color]++;
        if(PieceMinor[t_piece] == TRUE) t_minorPiece[color]++;

        t_material[color] += PieceValue[t_piece];
    }

    for(t_piece = wP; t_piece <= bK; ++t_piece) {
        ASSERT(t_pieceNum[t_piece] == pos->pieceNum[t_piece]);
    }

    pcount = CNT(t_pawns[WHITE]);
    ASSERT(pcount == pos->pieceNum[wP]);
    pcount = CNT(t_pawns[BLACK]);
    ASSERT(pcount == pos->pieceNum[bP]);
    pcount = CNT(t_pawns[BOTH]);
    ASSERT(pcount == (pos->pieceNum[bP] + pos->pieceNum[wP]));

    while(t_pawns[WHITE]) {
        sq64 = POP(&t_pawns[WHITE]);
        ASSERT(pos->pieces[SQ120(sq64)] == wP);
    }
    
        while(t_pawns[BLACK]) {
        sq64 = POP(&t_pawns[BLACK]);
        ASSERT(pos->pieces[SQ120(sq64)] == bP);
    }

    while(t_pawns[BOTH]) {
        sq64 = POP(&t_pawns[BOTH]);
        ASSERT(pos->pieces[SQ120(sq64)] == bP || pos->pieces[SQ120(sq64)] == wP);
    }

    ASSERT(t_material[WHITE] == pos->material[WHITE] && t_material[BLACK] == pos->material[BLACK]);
    ASSERT(t_minorPiece[WHITE] == pos->minorPiece[WHITE] && t_minorPiece[BLACK] == pos->minorPiece[BLACK]);
    ASSERT(t_majorPiece[WHITE] == pos->majorPiece[WHITE] && t_majorPiece[BLACK] == pos->majorPiece[BLACK]);
    ASSERT(t_bigPiece[WHITE] == pos->bigPiece[WHITE] && t_bigPiece[BLACK] == pos->bigPiece[BLACK]);

    ASSERT(pos->side == WHITE || pos->side == BLACK);
    ASSERT(GeneratePosKey(pos) == pos->posKey);

    ASSERT(pos->enPass == NO_SQ || (Rank[pos->enPass] == RANK_6 && pos->side == WHITE)
                                || (Rank[pos->enPass] == RANK_3 && pos->side == BLACK));

    ASSERT(pos->pieces[pos->KingSq[WHITE]] == wK);
    ASSERT(pos->pieces[pos->KingSq[BLACK]] == bK);

    return TRUE;
}

void UpdateListsMaterial(BOARD *pos) {

    int piece, sq, index, color;

    pos->material[WHITE] = pos->material[BLACK] = 0;

    for(index = 0; index < BOARD_SIZE; ++index) {
        sq = index;
        piece = pos->pieces[index];
        if(piece != OFFBOARD && piece != EMPTY) {
            color = PieceColor[piece];
            if(PieceBig[piece] == TRUE) pos->bigPiece[color]++;
            if(PieceMajor[piece] == TRUE) pos->majorPiece[color]++;
            if(PieceMinor[piece] == TRUE) pos->minorPiece[color]++;

            pos->material[color] += PieceValue[piece];

            pos->pieceList[piece][pos->pieceNum[piece]] = sq;
            pos->pieceNum[piece]++;

            if(piece == wK) pos->KingSq[WHITE] = sq;
            if(piece == bK) pos->KingSq[BLACK] = sq;

            if(piece == wP) {
                SETBIT(pos->pawns[WHITE], SQ64(sq));
                SETBIT(pos->pawns[BOTH], SQ64(sq));
            } else if(piece == bP) {
                SETBIT(pos->pawns[BLACK], SQ64(sq));
                SETBIT(pos->pawns[BOTH], SQ64(sq));
            }
        }
    }
}


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
            sq120 = FR2SQ(file, rank);
            sq64 = SQ64(sq120);
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
    
    UpdateListsMaterial(pos);

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

void PrintBoard(const BOARD *pos) {

    int sq, file, rank, piece;

    printf("\nGame Board:\n\n");

    for(rank = RANK_8; rank >= RANK_1; rank--) {
        printf("%d  ", rank + 1);
        for(file = FILE_A; file <= FILE_H; file++) {
            sq = FR2SQ(file, rank);
            piece = pos->pieces[sq];
            printf("%3c", PieceChar[piece]);
        }
        printf("\n");
    }

    printf("\n   ");
    for(file = FILE_A; file <= FILE_H; file++) {
        printf("%3c", FileChar[file]);
    }
    printf("\n");

    printf("side:%c\n", SideChar[pos->side]);
    printf("enPas:%d\n", pos->enPass);
    printf("castle:%c%c%c%c\n",
            pos->castlePerm & WKCA ? 'K' : '-',
            pos->castlePerm & WQCA ? 'Q' : '-',
            pos->castlePerm & BKCA ? 'k' : '-',
            pos->castlePerm & BQCA ? 'q' : '-'
          );
    printf("PosKey:%llX\n", pos->posKey);

}