#include "board.hpp"
#include <SFML/Graphics.hpp>

#include <vector>
#include <iostream>
#include <string>


int loadPosition(const Board& board, std::vector<sf::Sprite> &sprite_pieces){
    int idx = 0;
    std::vector<square_piece> sc_board = board.getSCBoard();

    for (int i = 0; i < 64; ++i){
        if (sc_board[i] == EM){
            continue;
        }
        else{
            int x = sc_board[i] % 6;
            int y = sc_board[i] <= 5 ? 0 : 1;

            int rank = i >> 3, file = i % 8;

            sprite_pieces[idx].setTextureRect(sf::IntRect(piece_size * x, piece_size * y, piece_size, piece_size));
            sprite_pieces[idx].setPosition(square_size * file, square_size * (7 - rank));
            idx++;
        }
    }
    return idx;
}


int main(){
    Board board = Board("r1bk3r/p2pBpNp/n4n2/1p1NP2P/6P1/3P4/P1P1K3/q5b1");

    std::vector<std::string> piece_string = {"WK", "WQ", "WB", "WN", "WR", "WP", "BK", "BQ", "BB", "BN", "BR", "BP", "EM"};
    std::vector<square_piece> sc_board = board.getSCBoard();
    for (int i = 7; i >= 0; --i){
        for (int j = 0; j < 8; ++j){
            std::cout << piece_string[(int)(sc_board[8 * i + j])] << " ";
        }
        std::cout << std::endl;
    }



    sf::RenderWindow window(sf::VideoMode(960, 960), "chess engine", sf::Style::Resize);

    sf::Texture piece_texture, board_texture;
    piece_texture.loadFromFile("images/pieces2.png");
    board_texture.loadFromFile("images/board.png");


    sf::Sprite sprite_board(board_texture);
    sprite_board.setScale(2.f, 2.f);

    std::vector<sf::Sprite> sprite_pieces(32);
    for(int i = 0; i < 32; ++i){
        sprite_pieces[i].setTexture(piece_texture);
        sprite_pieces[i].setScale(0.60f, 0.60f);
    }


    int num_pieces_remaining = loadPosition(board, sprite_pieces);

    while (window.isOpen()){
        sf::Event event;
        if(window.pollEvent(event)){
            if (event.type == sf::Event::Closed){
                window.close();
            }
        }

        window.clear();
        window.draw(sprite_board);
        for (int i = 0; i < num_pieces_remaining; ++i){
            window.draw(sprite_pieces[i]);
        }
        window.display();
    }

    return 0;

}