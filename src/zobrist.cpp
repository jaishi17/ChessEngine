#include "defs.hpp"
#include "zobrist.hpp"

#include <functional>
#include <iostream>
#include <random>
#include <bitset>
#include <chrono>


const int zobrist_size = 25;

u64 zobrist_keys[2][6][64];
u8 ztable[(1 << zobrist_size)];


void init_zobrist_keys(){

    std::random_device rd;
    std::mt19937_64 mt(rd());
    std::uniform_int_distribution<uint64_t> dist;

    auto drawFunc = std::bind(dist, mt);

    for (int i = 0; i < 2; ++i){
        for (int j = 0; j < 6; ++j){
            for (int k = 0; k < 64; ++k){
                zobrist_keys[i][j][k] = drawFunc();
            }
        }
    }

}