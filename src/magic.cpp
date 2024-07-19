#include "board.hpp"
#include "defs.hpp"

#include <functional>
#include <iostream>
#include <random>
#include <bitset>
#include <chrono>


u64 knight_table[64], king_table[64], pawn_push[2][64], pawn_attack[2][64];
u64 rook_magic[64], rook_mask[64], bishop_magic[64], bishop_mask[64];
std::vector<u64> rook_table[64], bishop_table[64];


void generate_pawn_table(){

    for (int i = 0; i < 64; ++i){
        pawn_push[0][i] = 0;
        pawn_attack[0][i] = 0;
        //white 
        if (i < 56){
            pawn_push[0][i] = bit_set_to(pawn_push[0][i], i + 8, 1);
            if ((int)i/8 == 1){
                pawn_push[0][i] = bit_set_to(pawn_push[0][i], i + 16, 1);
            }
            if (i % 8 != 0){
                pawn_attack[0][i] = bit_set_to(pawn_attack[0][i], i + 7, 1);
            }
            if (i % 8 != 7){
                pawn_attack[0][i] = bit_set_to(pawn_attack[0][i], i + 9, 1);
            }
        }
        
        pawn_push[1][i] = 0;
        pawn_attack[1][i] = 0;
        //black
        if (i > 7){
            pawn_push[1][i] = bit_set_to(pawn_push[1][i], i - 8, 1);
            if ((int)i/8 == 6){
                pawn_push[1][i] = bit_set_to(pawn_push[1][i], i - 16, 1);
            }
            if (i % 8 != 0){
                pawn_attack[1][i] = bit_set_to(pawn_attack[1][i], i - 9, 1);
            }
            if (i % 8 != 7){
                pawn_attack[1][i] = bit_set_to(pawn_attack[1][i], i - 7, 1);
            }
        }
    }

}

void generate_knight_table(){
    std::vector<int> moves = {10, 17, 15, 6, -10, -17, -15, -6};
    std::vector<u64> banned_files = {G_FILE | H_FILE, H_FILE, A_FILE, A_FILE | B_FILE, A_FILE | B_FILE, A_FILE, H_FILE, G_FILE | H_FILE};

    for (int i = 0; i < 64; ++i){
        knight_table[i] = 0;
        for (int k = 0; k < 8; ++k){
            if (0 <= i + moves[k] && i + moves[k] <= 63 && !(1 & (banned_files[k] >> i))){
                knight_table[i] = bit_set_to(knight_table[i], i + moves[k], 1);
            }
        }
    }
}

void generate_king_table(){
    std::vector<int> moves = {1, 9, 8, 7, -1, -9, -8, -7};
    std::vector<u64> banned_files = {H_FILE, H_FILE, (u64)0, A_FILE, A_FILE, A_FILE, (u64)0, H_FILE};

    for (int i = 0; i < 64; ++i){
        king_table[i] = 0;
        for (int k = 0; k < 8; ++k){
            if (0 <= i + moves[k] && i + moves[k] <= 63 && !(1 & (banned_files[k] >> i))){
                king_table[i] = bit_set_to(king_table[i], i + moves[k], 1);
            }
        }
    }
}

int rook_hash(u64 blockers, int square){
    return (blockers * rook_magic[square]) >> (rook_shift[square]);
}

int bishop_hash(u64 blockers, int square){
    return (blockers * bishop_magic[square]) >> bishop_shift[square];
}

u64 rook_moves(u64 blockers, int square){
    blockers &= rook_mask[square];
    int key = rook_hash(blockers, square);
    return rook_table[square][key];
}

u64 bishop_moves(u64 blockers, int square){
    blockers &= bishop_mask[square];
    int key = bishop_hash(blockers, square);
    return bishop_table[square][key];
}

u64 queen_moves(u64 blockers, int square){
    return bishop_moves(blockers, square) | rook_moves(blockers, square);
}


void generate_rook_mask(){
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

void generate_bishop_mask(){

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

u64 rook_attack(u64 blockers, int square){
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

u64 bishop_attack(u64 blockers, int square){
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

void generate_rook_magic(){

    std::random_device rd;
    std::mt19937_64 mt(rd());
    std::uniform_int_distribution<uint64_t> dist;

    auto drawFunc = std::bind(dist, mt);

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
            std::vector<bool> visited(1 << (64 - rook_shift[square]), false);
            // u64 possible_magic = distribution(generator) & distribution(generator) & distribution(generator); //want low bits
            u64 possible_magic = drawFunc() & drawFunc() & drawFunc();
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

void generate_bishop_magic(){
    std::random_device rd;
    std::mt19937_64 mt(rd());
    std::uniform_int_distribution<uint64_t> dist;

    auto drawFunc = std::bind(dist, mt);

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
            std::vector<bool> visited(1 << (64 - bishop_shift[square]), false);
            //u64 possible_magic = distribution(generator) & distribution(generator) & distribution(generator);
            u64 possible_magic = drawFunc() & drawFunc() & drawFunc();
            if (std::bitset<64>((blockers * possible_magic) & 0xFFF0000000000000ULL).count() < 6){
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


void init_magics(){
    for (int square = 0; square < 64; ++square){
        rook_table[square].resize(1 << (64 - rook_shift[square]));
        bishop_table[square].resize(1 << (64 - bishop_shift[square]));
    }
    generate_rook_mask();
    generate_rook_magic();

    generate_bishop_mask();
    generate_bishop_magic();
    // std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

    // generate_rook_mask();
    // std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    // generate_rook_magic();
    // std::chrono::steady_clock::time_point t3 = std::chrono::steady_clock::now();
    // generate_bishop_mask();
    // std::chrono::steady_clock::time_point t4 = std::chrono::steady_clock::now();
    // generate_bishop_magic();
    // std::chrono::steady_clock::time_point t5 = std::chrono::steady_clock::now();
    // std::cout << "rook mask = " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << "[ms]" << std::endl;
    // std::cout << "rook magic = " << std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count() << "[ms]" << std::endl;
    // std::cout << "bishop mask  = " << std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count() << "[ms]" << std::endl;
    // std::cout << "bishop magic = " << std::chrono::duration_cast<std::chrono::milliseconds>(t5 - t4).count() << "[ms]" << std::endl;

}
