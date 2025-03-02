#ifndef PIECE_H
#define PIECE_H
#include <cstdint>
#include <string>

enum class PieceType : uint8_t {
    EMPTY,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

struct Piece {
    PieceType type: 3;
    bool isWhite: 1;

    [[nodiscard]] std::string to_string() const;
    [[nodiscard]] bool is_empty() const;

    bool operator==(const Piece &other) const;
};

#endif //PIECE_H
