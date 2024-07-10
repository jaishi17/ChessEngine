#include "board.hpp"
#include "defs.hpp"

#include <iostream>
#include <random>
#include <bitset>

int Board::rook_hash(u64 blockers, int square){
    return (blockers * rook_magic[square]) >> (rook_shift[square]);
}

int Board::bishop_hash(u64 blockers, int square){
    return (blockers * bishop_magic[square]) >> bishop_shift[square];
}

u64 Board::rook_moves(u64 blockers, int square){
    blockers &= rook_mask[square];
    print_square(blockers);

    int key = rook_hash(blockers, square);

    std::cout << key << std::endl;

    return rook_table[square][key];
}

u64 Board::bishop_moves(u64 blockers, int square){
    blockers &= bishop_mask[square];
    int key = bishop_hash(blockers, square);
    return bishop_table[square][key];
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
    //bishop masks
    for (int i = 0; i < 64; ++i){
        bishop_mask[i] = u64(0);
        int rank = i >> 3, file = i % 8;

        std::vector<std::pair<int, int>> dir = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}};
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

    //rook magics
    for (int square = 0; square < 64; ++square){
        bool found_magic = false;
        int trials = 100000;
        u64 blockers = rook_mask[square];
        int num_blockers = std::bitset<64>(blockers).count();
        
        std::vector<u64> attacks(1 << num_blockers), idx (1 << num_blockers);
        u64 blocker_subset = 0;
        for (int i = 0; i < (1 << num_blockers); ++i){
            attacks[i] = rook_attack(blocker_subset, square);
            idx[i] = blocker_subset;
            blocker_subset = (blocker_subset - blockers) & blockers; //carry rippler trick
        }

        for (int k = 0; k < trials && !found_magic; ++k){
            std::vector<bool> visited(1 << rook_shift[square], false);
            //want low bits
            u64 possible_magic = distribution(generator) & distribution(generator) & distribution(generator);
            // std::cout << "pop count is " << std::bitset<64>(possible_magic).count() << std::endl;
            if(std::bitset<64>((blockers * possible_magic) & 0xFF00000000000000ULL).count() < 6) continue;

            rook_magic[square] = possible_magic;

            bool working_magic = true;

            //loop over configurations of blockers
            for (int i = 0; i < (1 << num_blockers); ++i){
                u64 attack = attacks[i];
                u64 subset = idx[i];
                int key = rook_hash(subset, square);
                if (visited[key] && rook_table[square][key] != attack){
                    working_magic = false;
                    break;
                }
                else{
                    visited[key] = true;
                    rook_table[square][key] = attack;
                }
                subset = (subset - blockers) & blockers; //carry rippler trick
            }
            if (working_magic){
                found_magic = true;
            }            
        }
        if (!found_magic){
            std::cout << "magic could not be found" << std::endl;
        }
        else{
            std::cout << "magic found for square " << square << std::endl; 
        }
    }
    
}

void Board::init_magics(){
    for (int square = 0; square < 64; ++square){
        rook_table[square].resize(1 << (64 - rook_shift[square]));
        bishop_table[square].resize(1 << (64 - bishop_shift[square]));
    }


    generate_rook_mask();
    generate_rook_magic();


    // generate_bishop_mask();
    // generate_bishop_magic();
}
