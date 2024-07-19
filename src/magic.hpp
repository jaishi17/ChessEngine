#ifndef magic_h
#define magic_h
#include "defs.hpp"



extern u64 knight_table[64], king_table[64], pawn_push[2][64], pawn_attack[2][64];
extern u64 rook_magic[64], rook_mask[64], bishop_magic[64], bishop_mask[64];
extern std::vector<u64> rook_table[64], bishop_table[64];



//leaping pieces
void generate_pawn_table();
void generate_knight_table();
void generate_king_table();

//sliding pieces magic stuff - in magic.cpp
void init_magics();

u64 rook_moves(u64 blockers, int square);
u64 bishop_moves(u64 blockers, int square);
u64 queen_moves(u64 blockers, int square);

int rook_hash(u64 blockers, int square);
int bishop_hash(u64 blockers, int square);

void generate_rook_mask();
void generate_rook_magic();

void generate_bishop_mask();
void generate_bishop_magic();

u64 rook_attack(u64 blockers, int square);
u64 bishop_attack(u64 blockers, int square);
        


#endif
