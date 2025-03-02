#include "engine/public_headers.h"
# include "spdlog/spdlog.h"
# include "chess/board.h"
# include "chess/piece.h"
#include <iostream>

void test() {
    spdlog::info("Hello World!");
    auto board = Board();
    std::cout << board.to_string() << std::endl;
    std::cout << board.to_string() << std::endl;
    auto out = board.find_piece(Piece{PieceType::KING, true});
    for (const auto& pair : out) {
        std::cout << "(" << pair.first << ", " << pair.second << ") ";
    }
    std::cout << std::endl;
}
