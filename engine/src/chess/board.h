#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <string>

#include "piece.h"

constexpr int BOARD_SIZE = 8;

class Board {
public:
    Board();

    [[nodiscard]] Piece get_piece(const std::pair<int, int> &pos) const;

    void set_piece(const std::pair<int, int> & pos, const Piece &piece);

    [[nodiscard]] std::string to_string() const;

    void make_raw_move(const std::pair<int, int> &src_pos,
                       const std::pair<int, int> &dst_pos);

    [[nodiscard]] std::vector<std::pair<int, int> > find_piece(const Piece &piece) const;

private:
    std::array<std::array<Piece, BOARD_SIZE>, BOARD_SIZE> board{};
};

#endif //BOARD_H
