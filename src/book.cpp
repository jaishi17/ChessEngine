
#include "defs.hpp"
#include "book.hpp"

#include <fstream>
#include <iostream> 
#include <vector>
#include <unordered_map>
#include <string>     


typedef uint8_t BYTE;



std::unordered_map<u64, Entry> book_moves;


void read_file()
{
    // open the file:
    std::ifstream file("/home/jaiden/projects/chess/engine/src/books/komodo.bin");

    // get its size:
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // read the data:


    unsigned char val;
    for (int i = 0; i < fileSize/16; ++i){
        u64 key = 0;
        short pre_sq = 0, post_sq = 0, weight = 0;
        for (int j = 0; j < 16; ++j){
            file.read(reinterpret_cast<char *>(&val), sizeof(val));
            // std::cout << std::hex << static_cast<int>(val) << " ";
            if (j <= 7){
                key += ((u64)val << (8 * (7 - j)));
            }
            if (j == 8){
                pre_sq = (val & 0xF) << 2;
            }
            else if (j == 9){
                pre_sq += ((val >> 6) & 0x3);
                post_sq = (val & 0x3F);
            }
            else if (j == 10){
                weight += (val << 8);
            }
            else if (j == 11){
                weight += val;
            }
        }
        if (book_moves.find(key) == book_moves.end() || book_moves[key].weight < weight){
            if (pre_sq == E1 && post_sq == H1){
                post_sq = G1;
            }
            else if (pre_sq == E1 && post_sq == A1){
                post_sq = C1;
            }
            else if (pre_sq == E8 && post_sq == H8){
                post_sq = G8;
            }
            else if (pre_sq == E8 && post_sq == A8){
                post_sq = C8;
            }
            book_moves[key] = {pre_sq, post_sq, weight};
        }

    }

    file.close();

}