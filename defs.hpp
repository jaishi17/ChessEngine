#ifndef defs_h
#define defs_h

const int color_count = 2;
const int piece_count = 6;

//pixel size for images
const int square_size = 120;
const int piece_size = 200;



enum piece_color {WHITE, BLACK};
enum piece_type {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING};
enum square_piece {WK, WQ, WB, WN, WR, WP, BK, BQ, BB, BN, BR, BP, EM};


#endif