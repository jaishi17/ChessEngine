#ifndef zobrist_h
#define zobrist_h
#include "defs.hpp"
#include "board.hpp"



typedef uint8_t u8;
extern const int zobrist_size;

extern u64 zobrist_keys[2][6][64];
extern u8 ztable[];


void init_zobrist_keys();


#endif
