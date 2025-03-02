#include "engine/public_headers.h"
# include "spdlog/spdlog.h"
# include "chess/board.h"
# include "chess/piece.h"
#include <iostream>

void test() {
    spdlog::info("Hello World!");
    auto board = Board();
    std::cout << board.to_string() << std::endl;
    board.make_raw_move(7, 4, 5, 4);
    std::cout << board.to_string() << std::endl;
}
