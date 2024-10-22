#ifndef SSE_H
#define SSE_H

#include <stdio.h>
#include <stdlib.h>

// #define DEBUG

#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if(!(n)) { \
printf("%s - Failed", #n); \
printf("On %s ", __DATE__); \
printf("At %s ", __TIME__); \
printf("In File %s ", __FILE__); \
printf("At Line %d\n", __LINE__); \
exit(1); }
#endif




typedef unsigned long long U64;

#define NAME "SSEHC 1.0"
#define BOARD_SIZE 120

#define MAX_GAME_MOVES 2048
#define MAX_POSITION_MOVES 256
#define MAX_DEPTH 64

#define START_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define INFINITE 30000
#define ISMATE (INFINITE - MAXDEPTH)



enum {EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK }; 
enum {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE};
enum {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE};

enum {WHITE, BLACK, BOTH};
enum {UCIMODE, XBOARDMODE, CONSOLEMODE}; 
enum {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFFBOARD
};

enum {FALSE, TRUE};

enum {WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8};


typedef struct {
    int move;
    int score;
} MOVE;

typedef struct {
    MOVE moves[MAX_POSITION_MOVES];
    int count;
} MOVE_LIST;

typedef struct {
    U64 posKey;
    int move;
} PV_ENTRY;

typedef struct {
    PV_ENTRY *pTable;
    int numEntries;
} PV_TABLE;

typedef struct {

    int move;
    int castlePerm;
    int enPass;
    int fiftyMove;
    U64 posKey;

} POSITION;



typedef struct {

    int pieces[BOARD_SIZE];
    U64 pawns[3];

    int KingSq[2];

    int side;
    int enPass;

    int fiftyMove;

    int ply;
    int hisPly;

    int castlePerm;

    U64 posKey;

    int pieceNum[13];
    int bigPiece[2];
    int majorPiece[2];
    int minorPiece[2];
    int material[2]; 


    POSITION history[MAX_GAME_MOVES];

    int pieceList[13][10];

    PV_TABLE PvTable[1]; 
    int PvArray[MAX_DEPTH];

    int searchHistory[13][BOARD_SIZE];
    int searchKillers[2][MAX_DEPTH];

} BOARD;

typedef struct {

    int starttime;
    int stoptime;
    int depth;
    int depthset;
    int timeset;
    int movestogo;
    int infinite;

    long nodes;

    int quit;
    int stopped;

    float fh;
    float fhf;

	int GAME_MODE;
	int POST_THINKING;

} SEARCHINFO;

// MOVE
#define FROMSQ(m) ((m) & 0x7F)
#define TOSQ(m) (((m) >> 7) & 0x7F)
#define CAPTURED(m) (((m) >> 14) & 0xF)
#define PROMOTED(m) (((m) >> 20) & 0xF)

#define MASK_EP 0x40000
#define MASK_PS 0x80000
#define MASK_CA 0x1000000 
 
#define MASK_CAP 0x7C000
#define MASK_PROM 0xF00000

#define NOMOVE 0 

// MACROS
#define FR2SQ(f, r) ( (21 + (f) ) + ( (r) * 10 ) )
#define SQ64(sq120) (Sq120ToSq64[sq120])
#define SQ120(sq64) (Sq64ToSq120[sq64])
#define POP(b) PopBit(b)
#define CNT(b) CountBits(b)
#define CLRBIT(bb, sq) ((bb) &= ClearMask[(sq)])
#define SETBIT(bb, sq) ((bb) |= SetMask[(sq)])

#define IsBQ(p) (PieceBishopQueen[(p)])
#define IsRQ(p) (PieceRookQueen[(p)])
#define IsN(p) (PieceKnight[(p)])
#define IsK(p) (PieceKing[(p)])


// Globals
extern int Sq120ToSq64[BOARD_SIZE];
extern int Sq64ToSq120[64];
extern U64 SetMask[64];
extern U64 ClearMask[64];
extern U64 PieceKeys[13][120];
extern U64 SideKey;
extern U64 CastleKeys[16];
extern char PieceChar[];
extern char SideChar[];
extern char RankChar[];
extern char FileChar[];

extern int PieceBig[13];
extern int PieceMajor[13];
extern int PieceMinor[13];
extern int PieceValue[13];
extern int PieceColor[13];

extern int File[BOARD_SIZE];
extern int Rank[BOARD_SIZE];

extern int PieceKnight[13];
extern int PieceKing[13];
extern int PieceRookQueen[13];
extern int PieceBishopQueen[13];
extern int PiecePawn[13];
extern int PieceSlides[13];

extern int Mirror64[64];


// Functions

// init.c
extern void Init();

// bitboard.c
extern void PrintBitBoard(U64 bb);
extern int PopBit(U64 *bb);
extern int CountBits(U64 b); 

// hashkey.c
extern U64 GeneratePosKey(const BOARD *pos);


// board.c
extern void ResetBoard(BOARD *pos);
extern int ParseFEN(char *fen, BOARD *pos);
extern void PrintBoard(const BOARD *pos);
extern void UpdateListsMaterial(BOARD *pos);
extern int CheckBoard(const BOARD *pos);
extern void MirrorBoard(BOARD *pos);

// attack.c
extern int SquareAttacked(const int sq, const int side, const BOARD *pos);

// io.c
extern char *SquareString(const int sq);
extern char *PrintMove(const int move);
extern void PrintMoveList(const MOVE_LIST *list);
extern int ParseMove(char *ptrChar, BOARD *pos);

// validate.c
extern int SquareOnBoard(const int sq);
extern int SideValid(const int side);
extern int FileRankValid(const int fr);
extern int PieceValidEmpty(const int pce);
extern int PieceValid(const int pce);
extern void MirrorEvalTest(BOARD *pos);

// genmove.c
extern void GenerateAllMoves(const BOARD *pos, MOVE_LIST *list);
extern void GenerateAllCaptures(const BOARD *pos, MOVE_LIST *list);
extern int MoveExists(BOARD *pos, const int move);
extern int InitMvvLva();

// makemove.c
extern int MakeMove(BOARD *pos, int move);
extern void TakeMove(BOARD *pos);

// perft.c
extern void PerftTest(int depth, BOARD *pos);

// search.c
extern void SearchPosition(BOARD *pos, SEARCHINFO *info); 

// misc.c
extern int GetTimeMs();
extern void ReadInput(SEARCHINFO *info);

// pvtable.c
extern void InitPvTable(PV_TABLE *table);
extern void StorePvMove(const BOARD *pos, const int move);
extern int ProbePvTable(const BOARD *pos);
extern int GetPvLine(const int depth, BOARD *pos);
extern void ClearPvTable(PV_TABLE *table);

// evaluate.c
extern int EvalPosition(const BOARD *pos);

// uci.c
extern void Uci_Loop(BOARD *pos, SEARCHINFO *info);
extern void ParsePosition(char *lineIn, BOARD *pos);

// xboard.c
extern void XBoard_Loop(BOARD *pos, SEARCHINFO *info);
extern void Console_Loop(BOARD *pos, SEARCHINFO *info);

#endif