#ifndef zobrist_h
#define zobrist_h
#include "defs.hpp"
#include "board.hpp"

struct TTentry{
   u64 z_key = 0;
   Move best_move;
   int depth;
   int eval;
   int eval_type; // 0 = exact, 1 = lower, 2 = upper

};


typedef uint8_t u8;
extern const int zobrist_size;

// extern u64 zobrist_keys[2][6][64];
extern const u64 zobrist_keys[];
extern TTentry ztable[];


// void init_zobrist_keys();


#endif
