#ifndef defs_h
#define defs_h

#include <cstdint>
#include <map>
#include <vector>

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
  51, 52, 52, 52, 52, 52, 52, 51,
  52, 53, 53, 53, 53, 53, 53, 52,
  52, 53, 53, 53, 53, 53, 53, 52,
  52, 53, 53, 53, 53, 53, 53, 52,
  52, 53, 53, 53, 53, 53, 53, 52,
  52, 53, 53, 53, 53, 53, 53, 52,
  52, 53, 53, 53, 53, 53, 53, 52,
  51, 52, 52, 52, 52, 52, 52, 51
};

const int bishop_shift[64] = {
  57, 58, 58, 58, 58, 58, 58, 57,
  58, 58, 58, 58, 58, 58, 58, 58,
  58, 58, 56, 56, 56, 56, 58, 58,
  58, 58, 56, 54, 54, 56, 58, 58,
  58, 58, 56, 54, 54, 56, 58, 58,
  58, 58, 56, 56, 56, 56, 58, 58,
  58, 58, 58, 58, 58, 58, 58, 58,
  57, 58, 58, 58, 58, 58, 58, 57
};




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


#endif