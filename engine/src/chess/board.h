#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <string>

#include "piece.h"

constexpr int BOARD_SIZE = 8;

class Board {
public:
    Board();

    [[nodiscard]] Piece get_piece(const int &row, const int &col) const;

    void set_piece(const int &row, const int &col, Piece piece);

    [[nodiscard]] std::string to_string() const;

    void make_raw_move(const int &src_row, const int &src_col, const int &dst_row, const int &dst_col);

private:
    std::array<std::array<Piece, BOARD_SIZE>, BOARD_SIZE> board{};
};

#endif //BOARD_H
