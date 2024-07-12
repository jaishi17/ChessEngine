#include "board.hpp"
#include "defs.hpp"

#include <iostream>
#include <random>
#include <bitset>
#include <chrono>


int Board::rook_hash(u64 blockers, int square){
    return (blockers * rook_magic[square]) >> (rook_shift[square]);
}

int Board::bishop_hash(u64 blockers, int square){
    return (blockers * bishop_magic[square]) >> bishop_shift[square];
}

u64 Board::rook_moves(u64 blockers, int square){
    blockers &= rook_mask[square];
    int key = rook_hash(blockers, square);
    return rook_table[square][key];
}

u64 Board::bishop_moves(u64 blockers, int square){
    blockers &= bishop_mask[square];
    int key = bishop_hash(blockers, square);
    return bishop_table[square][key];
}

u64 Board::queen_moves(u64 blockers, int square){
    return bishop_moves(blockers, square) | rook_moves(blockers, square);
}


void Board::generate_rook_mask(){
    //rook masks
    for (int i = 0; i < 64; ++i){
        rook_mask[i] = (u64)0;
        int rank = i >> 3, file = i % 8;
        for (int j = 1; j <= 6; ++j){
            rook_mask[i] = bit_set_to(rook_mask[i], 8 * rank + j, 1);
        }
        for (int j = 1; j <= 6; ++j){
            rook_mask[i] = bit_set_to(rook_mask[i], 8 * j + file, 1);
        }
        rook_mask[i] = bit_set_to(rook_mask[i], i, 0);
    }
}

void Board::generate_bishop_mask(){

    std::vector<std::pair<int, int>> dir = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}};

    //bishop masks
    for (int i = 0; i < 64; ++i){
        bishop_mask[i] = u64(0);
        int rank = i >> 3, file = i % 8;
        for (std::pair<int, int> k : dir){
            rank += k.first;
            file += k.second;
            while (1 <= rank && rank <= 6 && 1 <= file && file <= 6){
                bishop_mask[i] = bit_set_to(bishop_mask[i], 8 * rank + file, 1);
                rank += k.first;
                file += k.second;
            }
            rank = i >> 3;
            file = i % 8;
        }
    }
}

u64 Board::rook_attack(u64 blockers, int square){
    std::vector<std::pair<int, int>> dir = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};

    u64 attacks = 0;
    int rank = square >> 3, file = square % 8;

    for (std::pair<int, int> k : dir){
        rank += k.first;
        file += k.second;
        while (0 <= rank && rank <= 7 && 0 <= file && file <= 7){
            attacks = bit_set_to(attacks, 8 * rank + file, 1);
            if (blockers & ((u64)1 << (8 * rank + file))){
                break;
            }
            rank += k.first;
            file += k.second;
        }
        rank = square >> 3;
        file = square % 8;
    }

    return attacks;

}

u64 Board::bishop_attack(u64 blockers, int square){
    std::vector<std::pair<int, int>> dir = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

    u64 attacks = 0;
    int rank = square >> 3, file = square % 8;

    for (std::pair<int, int> k : dir){
        rank += k.first;
        file += k.second;
        while (0 <= rank && rank <= 7 && 0 <= file && file <= 7){
            attacks = bit_set_to(attacks, 8 * rank + file, 1);
            if (blockers & ((u64)1 << (8 * rank + file))){
                break;
            }
            rank += k.first;
            file += k.second;
        }
        rank = square >> 3;
        file = square % 8;
    }

    return attacks;
}

void Board::generate_rook_magic(){

    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<u64> distribution(0,0xFFFFFFFFFFFFFFFF);

    for (int square = 0; square < 64; ++square){
        bool found_magic = false;
        u64 blockers = rook_mask[square];
        int num_blockers = std::bitset<64>(blockers).count();
        std::vector<u64> attacks(1 << num_blockers), idx (1 << num_blockers);

        u64 blocker_subset = 0;
        for (int i = 0; i < (1 << num_blockers); ++i){
            attacks[i] = rook_attack(blocker_subset, square);
            idx[i] = blocker_subset;
            blocker_subset = (blocker_subset - blockers) & blockers; //carry rippler trick
        }

        while (!found_magic){
            std::vector<bool> visited(1 << rook_shift[square], false);
            u64 possible_magic = distribution(generator) & distribution(generator) & distribution(generator); //want low bits
            if(std::bitset<64>((blockers * possible_magic) & 0xFFF0000000000000ULL).count() < 6){
                continue;
            }
            rook_magic[square] = possible_magic;
            bool working_magic = true;

            //loop over configurations of blockers
            for (int i = 0; i < (1 << num_blockers); ++i){
                u64 attack = attacks[i], subset = idx[i];
                int key = rook_hash(subset, square);
                if (visited[key] && rook_table[square][key] != attack){
                    working_magic = false;
                    break;
                }
                else{
                    visited[key] = true;
                    rook_table[square][key] = attack;
                }
            }
            found_magic = working_magic;   
        }
    }

}

void Board::generate_bishop_magic(){
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<u64> distribution(0,0xFFFFFFFFFFFFFFFF);

    for (int square = 0; square < 64; ++square){
        bool found_magic = false;
        u64 blockers = bishop_mask[square];
        int num_blockers = std::bitset<64>(blockers).count();
        std::vector<u64> attacks(1 << num_blockers), idx(1 << num_blockers);

        u64 blocker_subset = 0;
        for (int i = 0; i < (1 << num_blockers); ++i){
            attacks[i] = bishop_attack(blocker_subset, square);
            idx[i] = blocker_subset;
            blocker_subset = (blocker_subset - blockers) & blockers;
        }

        while (!found_magic){
            std::vector<bool> visited(1 << bishop_shift[square], false);
            u64 possible_magic = distribution(generator) & distribution(generator) & distribution(generator);
            if (std::bitset<64>((blockers * possible_magic) & 0xFFF0000000000000).count() < 6){
                continue;
            }
            bishop_magic[square] = possible_magic;
            bool working_magic = true;

            for (int i = 0; i < (1 << num_blockers); ++i){
                u64 attack = attacks[i], subset = idx[i];
                int key = bishop_hash(subset, square);
                if (visited[key] && bishop_table[square][key] != attack){
                    working_magic = false;
                    break;
                }
                else{
                    visited[key] = true;
                    bishop_table[square][key] = attack;
                }

            }
            found_magic = working_magic;
        }

    }
}


void Board::init_magics(){
    for (int square = 0; square < 64; ++square){
        rook_table[square].resize(1 << (64 - rook_shift[square]));
        bishop_table[square].resize(1 << (64 - bishop_shift[square]));
    }

    // std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    generate_rook_mask();
    generate_rook_magic();


    generate_bishop_mask();
    generate_bishop_magic();

    // std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

}
