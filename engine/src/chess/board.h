#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <optional>
#include <string>

#include "piece.h"

constexpr int BOARD_SIZE{8};

class Board {
public:
    Board();

    [[nodiscard]] Piece get_piece(const std::pair<int, int> &pos) const;

    void set_piece(const std::pair<int, int> & pos, const Piece &piece);

    [[nodiscard]] std::string to_string() const;

    void make_raw_move(const std::pair<int, int> &src_pos,
                       const std::pair<int, int> &dst_pos);

    [[nodiscard]] std::vector<std::pair<int, int> > find_piece(const Piece &piece) const;

    std::vector<std::pair<int, int>> get_valid_moves(const std::pair<int, int> &pos, const bool &attack_moves_only, const bool &validate_pin);

private:
    std::array<std::array<Piece, BOARD_SIZE>, BOARD_SIZE> board{};
    std::optional<std::pair<int, int>> en_passant{};
};

#endif //BOARD_H
