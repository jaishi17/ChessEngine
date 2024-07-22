#ifndef defs_h
#define defs_h

#include <cstdint>
#include <map>
#include <vector>
#include <string> 
#include <iostream>

typedef uint64_t u64;


const int color_count = 2;
const int piece_count = 6;

//pixel size for images
const int square_size = 120;
const int piece_size = 200;


const u64 RANK_1 = 0xffull;
const u64 RANK_2 = 0xff00ull;
const u64 RANK_3 = 0xff0000ull;
const u64 RANK_4 = 0xff000000ull;
const u64 RANK_5 = 0xff00000000ull;
const u64 RANK_6 = 0xff0000000000ull;
const u64 RANK_7 = 0xff000000000000ull;
const u64 RANK_8 = 0xff00000000000000ull;
const std::vector<u64> rank_constants = {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8};


const u64 A_FILE = 0x101010101010101ull;
const u64 B_FILE = 0x202020202020202ull;
const u64 C_FILE = 0x404040404040404ull;
const u64 D_FILE = 0x808080808080808ull;
const u64 E_FILE = 0x1010101010101010ull;
const u64 F_FILE = 0x2020202020202020ull;
const u64 G_FILE = 0x4040404040404040ull;
const u64 H_FILE = 0x8080808080808080ull;
const std::vector<u64> file_constants = {A_FILE, B_FILE, C_FILE, D_FILE, E_FILE, F_FILE, G_FILE, H_FILE};

const int rook_shift[64] = {
  52, 53, 53, 53, 53, 53, 53, 52,
  53, 54, 54, 54, 54, 54, 54, 53,
  53, 54, 54, 54, 54, 54, 54, 53,
  53, 54, 54, 54, 54, 54, 54, 53,
  53, 54, 54, 54, 54, 54, 54, 53,
  53, 54, 54, 54, 54, 54, 54, 53,
  53, 54, 54, 54, 54, 54, 54, 53,
  52, 53, 53, 53, 53, 53, 53, 52
};

const int bishop_shift[64] = {
  58, 59, 59, 59, 59, 59, 59, 58,
  59, 59, 59, 59, 59, 59, 59, 59,
  59, 59, 55, 55, 55, 55, 59, 59,
  59, 59, 55, 55, 55, 55, 59, 59,
  59, 59, 55, 55, 55, 55, 59, 59,
  59, 59, 55, 55, 55, 55, 59, 59,
  59, 59, 59, 59, 59, 59, 59, 59,
  58, 59, 59, 59, 59, 59, 59, 58
};

const int castle_rooks[4] = {7, 0, 63, 56}; 


enum piece_color {WHITE, BLACK, NONE};
enum piece_type {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, ALL};
// enum square_piece {WK, WQ, WB, WN, WR, WP, BK, BQ, BB, BN, BR, BP, EM};

enum squares_RF {
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2, 
  A3, B3, C3, D3, E3, F3, G3, H3, 
  A4, B4, C4, D4, E4, F4, G4, H4, 
  A5, B5, C5, D5, E5, F5, G5, H5, 
  A6, B6, C6, D6, E6, F6, G6, H6, 
  A7, B7, C7, D7, E7, F7, G7, H7, 
  A8, B8, C8, D8, E8, F8, G8, H8 
};

const char pc_str[3] = {'W', 'B', ' '};

const char pt_str[7] = {'P', 'N', 'B', 'R', 'Q', 'K',' '};

const std::string squares_RF_str[64] {
  "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
  "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2", 
  "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", 
  "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", 
  "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5", 
  "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6", 
  "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7", 
  "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8" 
};

inline u64 bit_set_to(u64 number, u64 n, bool x) {
	return (number & ~((u64)1 << n)) | ((u64)x << n);
}

inline bool get_bit(u64 number, int pos){
    return (number >> pos) & 1;
}

const int lsb_index[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};

const u64 debruijn64 = (u64)0x03f79d71b4cb0a89;

inline int lsb(u64 n){
   	return lsb_index[((n ^ (n-1)) * debruijn64) >> 58];
}




const char uci_promo[6] = {'p', 'n', 'b', 'r', 'q', 'k'};

extern const int piece_value_mid[6];
extern const int piece_value_end[6];
extern const int perspective[2];

extern int pawn_pst_mid[64], pawn_pst_end[64];
extern int knight_pst_mid[64], knight_pst_end[64];
extern int bishop_pst_mid[64], bishop_pst_end[64];
extern int rook_pst_mid[64], rook_pst_end[64];
extern int queen_pst_mid[64], queen_pst_end[64];
extern int king_pst_mid[64], king_pst_end[64];



extern const int * piece_pst_mid[6];
extern const int * piece_pst_end[6];
extern const int piece_phase[6];

#endif