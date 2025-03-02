#include "piece.h"

std::string Piece::to_string() const {
    switch (type) {
        case PieceType::EMPTY:
            return ".";
        case PieceType::PAWN:
            return isWhite ? "♟" : "♙";
        case PieceType::ROOK:
            return isWhite ? "♜" : "♖";
        case PieceType::KNIGHT:
            return isWhite ? "♞" : "♘";
        case PieceType::BISHOP:
            return isWhite ? "♝" : "♗";
        case PieceType::QUEEN:
            return isWhite ? "♛" : "♕";
        case PieceType::KING:
            return isWhite ? "♚" : "♔";
        default:
            throw std::domain_error("Unknown piece type");
    }
}

bool Piece::is_empty() const {
    if (type == PieceType::EMPTY) {
        return true;
    }
    return false;
}

bool Piece::operator==(const Piece &other) const {
    return std::tie(type, isWhite) == std::tie(other.type, isWhite);
}
