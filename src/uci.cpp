#include "uci.hpp"
#include "board.hpp"

#include <iostream>
#include <sstream>
#include <unordered_map> 

bool quit_uci = false;

std::vector<std::string> split_input(std::string s){
    std::vector<std::string> res;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, ' ')){
        res.push_back(item);
    }
    return res;
}

std::unordered_map<std::string, int> uci_commands = {
    {"uci", 0}, 
    {"debug", 1}, 
    {"isready", 2},
    {"setoption", 3}, 
    {"register", 4}, 
    {"ucinewgame", 5}, 
    {"position", 6}, 
    {"go", 7}, 
    {"stop", 8}, 
    {"ponderhit", 9}, 
    {"quit", 10}, 
};


void uci_uci(){
    std::cout << "id name js_engine 071624\n";
    std::cout << "id author js\n";
    std::cout << "uciok\n";
    
};

void uci_debug(){
    
}

void uci_isready(){
    std::cout << "readyok\n";

}

void uci_setoption(){
    
}

void uci_register(){
    
}

void uci_ucinewgame(Board &board){
    board = Board();
}

void uci_position(Board &board, std::vector<std::string> commands){
    size_t idx = 1;
    if (idx < commands.size()){
        std::string arg = commands[idx++];
        if (arg == "startpos"){
            board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ");
        }
        else if (arg == "fen"){
            std::string fen_string = ""; 
            while (commands[idx] != "moves" && idx < commands.size()){
                fen_string += commands[idx];
                idx++;
            }
            board = Board(fen_string);
        }
        idx++;
        while (idx < commands.size()){
            // std::cout << "A)" << commands[idx] << ": "  << std::hex << board.zhash_pos << std::endl;
            board.update(commands[idx]);
            // std::cout << "B)" << commands[idx] << ": "  << std::hex << board.zhash_pos << std::endl;

            idx++;
        }

    }
    // std::cout << std::hex << board.zhash_pos << std::endl;
}

void uci_go(Board &board, std::vector<std::string> commands){
    size_t idx = 1;
    int depth = -1, time = -1;
    while (idx < commands.size()){
        std::string arg = commands[idx++];
        if (arg == "wtime"){
            board.set_time(stoi(commands[idx++]), 0);
        }
        else if (arg == "btime"){
            board.set_time(stoi(commands[idx++]), 1);
        }
        else if (arg == "winc"){
            board.set_time_inc(stoi(commands[idx++]), 0);
        }
        else if (arg == "binc"){
            board.set_time_inc(stoi(commands[idx++]), 1);
        }
        else if (arg == "movestogo"){
            board.set_movestogo(stoi(commands[idx++]));
        }
        else if (arg == "depth"){
            depth = stoi(commands[idx++]);
        }
        else if (arg == "movetime"){
            time = stoi(commands[idx++]);
        }
    }
    board.engine_move(depth, time);
}

void uci_stop(){
    
}
void uci_ponderhit(){
    
}
void uci_quit(){
    quit_uci = true;
}

void uci_loop(){

    Board board;

    std::string input;
    while (!quit_uci && std::getline(std::cin, input)){
        std::vector<std::string> commands = split_input(input);
        switch (uci_commands[commands[0]]){
            case 0:
                uci_uci();
                break;
            case 1:    
                uci_debug();
                break;
            case 2: 
                uci_isready();
                break;
            case 3: 
                uci_setoption();
                break;
            case 4: 
                uci_register();
                break;
            case 5: 
                uci_ucinewgame(board);
                break;
            case 6: 
                uci_position(board, commands);
                break;
            case 7: 
                uci_go(board, commands);
                break;
            case 8: 
                uci_stop();
                break;
            case 9: 
                uci_ponderhit();
                break;
            case 10: 
                uci_quit();
                break;
            
            

        }
    }

}