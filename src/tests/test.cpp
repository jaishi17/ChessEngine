#include "../board.hpp"
#include "../magic.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>   
#include <chrono>   


std::vector<std::string> split_perft_result(std::string s){
    std::vector<std::string> res;
    res.reserve(7);
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, ';')){
        res.push_back(item);
    }
    return res;

}


int main2(){

    generate_pawn_table();
    generate_knight_table();
    generate_king_table();
    init_magics();

    std::string start_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

    Board board = Board(start_position);

    u64 res = board.perft(6, true);
    std::cout << res << std::endl;

    return 0;
}

int main(){

    generate_pawn_table();
    generate_knight_table();
    generate_king_table();
    init_magics();

    std::ifstream file("tests/perft_results.txt");
    std::string str;
    int idx = 1; 

    int total_passed = 0, total_total = 0, positions_passed = 0;
    u64 total_nodes = 0;

    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    
    while (std::getline(file, str)){
        std::cout << "Checking position " << idx  << std::endl;


        std::vector<std::string> split_perft = split_perft_result(str);

        std::string fen = split_perft[0];

        u64 results[6] = {0, 0, 0, 0, 0, 0};
        for(int i = 0; i < 6; ++i){
            results[(int)(split_perft[i + 1][1] - '1')] = std::stoull(split_perft[i + 1].substr(3));
        } 


        Board board = Board(fen);

        int passed = 0, total = 0;
        std::vector<int> failed = {};
        std::vector<u64> res(6);

        for (int i = 0; i < 6; ++i){
            if (!(idx == 2 && i == 5)){
                res[i] = board.perft(i + 1);
            }
            if (res[i] == results[i]){
                passed++;
            }
            else{
                failed.push_back(i + 1);
            }
            total_nodes += res[i];
            total++;
        
        }
        if (passed != total){
            std::cout << fen << std::endl;
            std::cout << "For position " << idx  << ": (" << passed << " / " << total << ")\n";
            std::cout << "Failed depths: ";
            for (int x : failed){
                std::cout << x << " ";
            }
            std::cout << std::endl;
            for (int i = 0; i < 6; ++i){
                std::cout << res[i] << " " << results[i] << std::endl;
            }
            std::cout << "\n" << std::endl;
        }
        else{
            positions_passed++;
        }
        total_passed += passed;
        total_total += total;
        idx++;
    }
    
    
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    int duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "Time difference = " << duration << "[ms]" << std::endl;

    std::cout << "Move generation speed: " << total_nodes / duration << "[knps]" << std::endl; 

    std::cout << "Passed: " << total_passed << "/" << total_total << std::endl;
    std::cout << "positions passed: " << positions_passed << std::endl;
    return 0;
}