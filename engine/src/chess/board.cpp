#include <algorithm>
#include <vector>

#include "board.h"

Board::Board() {
    // Initialize to be empty
    for (std::array<Piece, BOARD_SIZE> &row: board) {
        std::ranges::fill(row, Piece{PieceType::EMPTY, true});
    }

    // Initialize black pieces
    board[0][0] = board[0][7] = Piece{PieceType::ROOK, false};
    board[0][1] = board[0][6] = Piece{PieceType::KNIGHT, false};
    board[0][2] = board[0][5] = Piece{PieceType::BISHOP, false};
    board[0][3] = Piece{PieceType::QUEEN, false};
    board[0][4] = Piece{PieceType::KING, false};
    std::ranges::fill(board[1], Piece{PieceType::PAWN, false});

    // Initialize white pieces
    board[7][0] = board[7][7] = Piece{PieceType::ROOK, true};
    board[7][1] = board[7][6] = Piece{PieceType::KNIGHT, true};
    board[7][2] = board[7][5] = Piece{PieceType::BISHOP, true};
    board[7][3] = Piece{PieceType::QUEEN, true};
    board[7][4] = Piece{PieceType::KING, true};
    std::ranges::fill(board[6], Piece{PieceType::PAWN, true});
}

Piece Board::get_piece(const std::pair<int, int> &pos) const {
    if (pos.first >= 0 && pos.first < BOARD_SIZE && pos.second >= 0 && pos.
        second < BOARD_SIZE) {
        return board[pos.first][pos.second];
    }
    const std::string error_msg =
            "Invalid row or column specified (board size: " +
            std::to_string(BOARD_SIZE) + "x" + std::to_string(BOARD_SIZE) + ")";
    throw std::out_of_range(error_msg);
}

void Board::set_piece(const std::pair<int, int> &pos, const Piece &piece) {
    if (pos.first >= 0 && pos.first < BOARD_SIZE && pos.second >= 0 && pos.
        second < BOARD_SIZE) {
        board[pos.first][pos.second] = piece;
    } else {
        const std::string error_msg =
                "Invalid row or column specified (board size: " +
                std::to_string(BOARD_SIZE) + "x" + std::to_string(BOARD_SIZE) +
                ")";
        throw std::out_of_range(error_msg);
    }
}

std::string Board::to_string() const {
    std::string result = "  ┌";

    for (int i = 0; i < BOARD_SIZE * 2 + 1; ++i) {
        result += "─";
    }
    result += "┐\n";

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (j == 0) {
                result += std::to_string(8 - i) + " │ ";
            }
            result += board[i][j].to_string() + " ";
            if (j == BOARD_SIZE - 1) {
                result += "│";
            }
        }
        result += "\n";
    }

    result += "  └";
    for (int i = 0; i < BOARD_SIZE * 2 + 1; ++i) {
        result += "─";
    }
    result += "┘\n    ";

    for (int i = 0; i < BOARD_SIZE; ++i) {
        result += std::string(1, static_cast<char>('a' + i)) + " ";
    }

    return result;
}

void Board::make_raw_move(const std::pair<int, int> &src_pos,
                          const std::pair<int, int> &dst_pos) {
    if (src_pos.first >= 0 && src_pos.first < BOARD_SIZE &&
        src_pos.second >= 0 && src_pos.second < BOARD_SIZE &&
        dst_pos.first >= 0 && dst_pos.first < BOARD_SIZE &&
        dst_pos.second >= 0 && dst_pos.second < BOARD_SIZE) {
        board[dst_pos.first][dst_pos.second] = board[src_pos.first][src_pos.
            second];
        board[src_pos.first][src_pos.second] = Piece{
            PieceType::EMPTY, false
        };
    } else {
        const std::string error_msg =
                "Invalid row or col specified (board size: " +
                std::to_string(BOARD_SIZE) + "x" + std::to_string(BOARD_SIZE) +
                ")";
        throw std::out_of_range(error_msg);
    }
}

std::vector<std::pair<int, int> > Board::find_piece(const Piece &piece) const {
    std::vector<std::pair<int, int> > result;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (piece == board[i][j]) {
                result.emplace_back(i, j);
            }
        }
    }

    return result;
}
