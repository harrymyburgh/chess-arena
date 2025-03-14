#include <algorithm>
#include <vector>
#include <ranges>

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

    // Initialize control variables
    white_king_side_castle = white_queen_side_castle =
                             black_king_side_castle =
                             black_queen_side_castle = true;
    white_turn = true;
}

Piece Board::get_piece(const std::pair<int, int> &pos) const {
    if (pos.first >= 0 && pos.first < BOARD_SIZE && pos.second >= 0 && pos.
        second < BOARD_SIZE) {
        return board[pos.first][pos.second];
    }
    const std::string error_msg{
        "Invalid row or column specified (board size: " +
        std::to_string(BOARD_SIZE) + "x" + std::to_string(BOARD_SIZE) + ")"
    };
    throw std::out_of_range(error_msg);
}

void Board::set_piece(const std::pair<int, int> &pos, const Piece &piece) {
    if (pos.first >= 0 && pos.first < BOARD_SIZE && pos.second >= 0 && pos.
        second < BOARD_SIZE) {
        board[pos.first][pos.second] = piece;
    } else {
        const std::string error_msg{
            "Invalid row or column specified (board size: " +
            std::to_string(BOARD_SIZE) + "x" + std::to_string(BOARD_SIZE) + ")"
        };
        throw std::out_of_range(error_msg);
    }
}

std::string Board::to_string() const {
    std::string result{"  ┌"};

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

void Board::make_move_raw(const std::pair<int, int> &src_pos,
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
        const std::string error_msg{
            "Invalid row or col specified (board size: " +
            std::to_string(BOARD_SIZE) + "x" + std::to_string(BOARD_SIZE) +
            ")"
        };
        throw std::out_of_range(error_msg);
    }
}

void Board::make_move(const std::pair<int, int> &src_pos,
                      const std::pair<int, int> &dst_pos,
                      const std::optional<PieceType> promotion_piece =
                              std::nullopt) {
    if (src_pos.first < 0 || src_pos.first >= BOARD_SIZE ||
        src_pos.second < 0 || src_pos.second >= BOARD_SIZE ||
        dst_pos.first < 0 || dst_pos.first >= BOARD_SIZE ||
        dst_pos.second < 0 || dst_pos.second >= BOARD_SIZE) {
        const std::string error_msg{
            "Invalid row or col specified (board size: " +
            std::to_string(BOARD_SIZE) + "x" + std::to_string(BOARD_SIZE) +
            ")"
        };
        throw std::out_of_range(error_msg);
    }

    if (src_pos.first == dst_pos.first && src_pos.second == dst_pos.second) {
        const std::string error_msg{
            "Cannot have a destination position that is the same as the start position"
        };
        throw std::out_of_range(error_msg);
    }

    if (promotion_piece.has_value()) {
        if (promotion_piece.value() != PieceType::ROOK || promotion_piece.
            value() != PieceType::KNIGHT || promotion_piece.value() !=
            PieceType::BISHOP || promotion_piece.value() != PieceType::QUEEN) {
            const std::string error_msg{"Invalid promotion piece specified"};
            throw std::invalid_argument(error_msg);
        }
    }

    if (board[src_pos.first][src_pos.second].type == PieceType::EMPTY) {
        const std::string error_msg{"Empty start square specified"};
        throw std::invalid_argument(error_msg);
    }

    if ((board[src_pos.first][src_pos.second].type == PieceType::PAWN && board[
             src_pos.first][src_pos.second].isWhite && dst_pos.first == 0 && !
         promotion_piece.has_value()) || (
            board[src_pos.first][src_pos.second].type == PieceType::PAWN && !
            board[src_pos.first][src_pos.second].isWhite && dst_pos.first == 7
            && !promotion_piece.has_value())) {
        const std::string error_msg{
            "Promotion piece not specified for pawn promotion"
        };
        throw std::invalid_argument(error_msg);
    }

    if (board[src_pos.first][src_pos.second].isWhite != white_turn) {
        const std::string error_msg{
            "Cannot this color piece on this turn (white turn: " +
            std::to_string(white_turn) + ", white piece: " + std::to_string(
                board[src_pos.first][src_pos.second].isWhite) + ")"
        };
        throw std::invalid_argument(error_msg);
    }

    std::vector<std::pair<int, int> > valid_moves = get_valid_moves_raw(
        src_pos, false, true);
    // TODO continue with move logic
}

std::vector<std::pair<int, int> > Board::find_piece(const Piece &piece) const {
    std::vector<std::pair<int, int> > result{};
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (piece == board[i][j]) {
                result.emplace_back(i, j);
            }
        }
    }

    return result;
}

std::vector<std::pair<int, int> > Board::get_valid_moves_raw(
    const std::pair<int, int> &pos, const bool &attack_moves_only,
    const bool &validate_pin) {
    if (pos.first < 0 || pos.first >= BOARD_SIZE || pos.second < 0 || pos.
        second >= BOARD_SIZE) {
        const std::string error_msg{
            "Invalid row or column specified (board size: " +
            std::to_string(BOARD_SIZE) + "x" + std::to_string(BOARD_SIZE) + ")"
        };
        throw std::out_of_range(error_msg);
    }

    std::vector<std::pair<int, int> > moves{};
    auto &[row, col]{pos};
    auto &[type, isWhite]{board[row][col]};

    // Helper function for checking bounds of a row and column.
    auto is_inside = [](const int &r, const int &c) {
        return (0 <= r && r < BOARD_SIZE) && (0 <= c && c < BOARD_SIZE);
    };

    switch (type) {
        // ---------------------- Empty Square ----------------------
        case PieceType::EMPTY:
            return moves;

        // ----------------------- Pawn Moves -----------------------
        case PieceType::PAWN: {
            // For white pawns, they move upward (i.e. decreasing row index);
            // for black pawns, they move downward.
            const int direction = isWhite ? -1 : 1;
            const int start_row = isWhite ? 6 : 1;

            if (!attack_moves_only) {
                // Move forward one.
                if (const int forward{row + direction};
                    is_inside(forward, col) && board[forward][col].is_empty()) {
                    moves.emplace_back(forward, col);
                    // If on starting rank, pawn can move two squares forward.
                    if (row == start_row) {
                        if (const int forward2{row + 2 * direction};
                            is_inside(forward2, col) && board[forward2][col].
                            is_empty()) {
                            moves.emplace_back(forward2, col);
                        }
                    }
                }
            }

            for (const int &dc: std::array{-1, 1}) {
                if (int new_c{col + dc}, new_r{row + direction}; is_inside(
                    new_r, new_c)) {
                    if (Piece &target{board[new_r][new_c]};
                        !target.is_empty() && target.isWhite != isWhite) {
                        moves.emplace_back(new_r, new_c);
                    }
                    // Check en passant: if en_passant is set and matches the candidate square.
                    if (!en_passant.has_value()) {
                        if (new_r == en_passant->first && new_c == en_passant->
                            second) {
                            moves.emplace_back(new_r, new_c);
                        }
                    }
                }
            }
            break;
        }

        // ----------------------- Knight Moves -----------------------
        case PieceType::KNIGHT: {
            constexpr std::array<std::pair<int, int>, 8> knight_offsets{
                {
                    {2, 1},
                    {2, -1},
                    {-2, 1},
                    {-2, -1},
                    {1, 2},
                    {1, -2},
                    {-1, 2},
                    {-1, -2}
                }
            };

            for (const auto &[dr, dc]: knight_offsets) {
                if (int new_r{row + dr}, new_c{col + dc}; is_inside(
                    new_r, new_c)) {
                    if (Piece &target{board[new_r][new_c]};
                        target.is_empty() || target.isWhite != isWhite) {
                        moves.emplace_back(new_r, new_c);
                    }
                }
            }
            break;
        }

        // ------------------------ Rook Moves ------------------------
        case PieceType::ROOK:
            [[fallthrough]];
        // ----------------------- Bishop Moves -----------------------
        case PieceType::BISHOP:
            [[fallthrough]];
        // ----------------------- Queen Moves ------------------------
        case PieceType::QUEEN: {
            constexpr std::array<std::pair<int, int>, 4> rank_file_directions{
                {
                    {-1, 0},
                    {1, 0},
                    {0, -1},
                    {0, 1}
                }
            };
            constexpr std::array<std::pair<int, int>, 4> diag_directions{
                {
                    {-1, -1},
                    {-1, 1},
                    {1, -1},
                    {1, 1}
                }
            };

            std::vector<const std::array<std::pair<int, int>, 4> *>
                    direction_types;

            if (type == PieceType::ROOK || type ==
                PieceType::QUEEN) {
                direction_types.push_back(&rank_file_directions);
            }
            if (type == PieceType::BISHOP || type ==
                PieceType::QUEEN) {
                direction_types.push_back(&diag_directions);
            }

            for (const auto *directions: direction_types) {
                for (const auto &[dr, dc]: *directions) {
                    int r{row}, c{col};

                    while (true) {
                        r += dr;
                        c += dc;
                        if (!is_inside(r, c)) {
                            break;
                        }
                        if (Piece &target{board[r][c]}; target.is_empty()) {
                            moves.emplace_back(r, c);
                        } else if (target.isWhite != isWhite) {
                            moves.emplace_back(r, c);
                            break;
                        } else {
                            break;
                        }
                    }
                }
            }
            break;
        }
        // ------------- King Moves (including castling) --------------
        case PieceType::KING: {
            std::array<std::pair<int, int>, 8> king_offsets{
                {
                    {-1, -1},
                    {-1, 0},
                    {-1, 1},
                    {0, -1},
                    {0, 1},
                    {1, -1},
                    {1, 0},
                    {1, 1}
                }
            };

            for (const auto &[dr, dc]: king_offsets) {
                if (int new_r{row + dr}, new_c{col + dc}; is_inside(
                    new_r, new_c)) {
                    if (Piece &target{board[new_r][new_c]};
                        !target.is_empty() || target.isWhite != isWhite) {
                        moves.emplace_back(new_r, new_c);
                    }
                }
            }

            if (!attack_moves_only) {
                // For castling, we check that the king is at its starting square.
                if (isWhite && row == 7 && col == 4) {
                    // White king-side castling: squares f1 and g1 must be empty
                    // and the rook must be at h1.
                    if (white_king_side_castle && board[7][5].is_empty() &&
                        board[7][6].is_empty() &&
                        board[7][7] == Piece{PieceType::ROOK, true} &&
                        is_under_attack(
                            std::vector{
                                {
                                    std::pair{7, 5},
                                    std::pair{7, 6}
                                }
                            },
                            false, true)) {
                        moves.emplace_back(7, 6);
                    }
                    // White queen-side castling: squares d1, c1, and b1 must be empty
                    // and the rook must be at a1.
                    if (white_queen_side_castle && board[7][3].is_empty() &&
                        board[7][2].is_empty() && board[7][1].is_empty() &&
                        board[7][0] == Piece{PieceType::ROOK, true} &&
                        is_under_attack(std::vector{
                                            {
                                                std::pair{7, 3},
                                                std::pair{7, 2},
                                                std::pair{7, 1}
                                            }
                                        }, false, true)) {
                        moves.emplace_back(7, 2);
                    }
                    // Black king-side castling: squares f8 and g8 must be empty and
                    // the rook must be at h8.
                    if (black_king_side_castle && board[0][5].is_empty() &&
                        board[0][6].is_empty() &&
                        board[0][7] == Piece{PieceType::ROOK, false} &&
                        is_under_attack(
                            std::vector{
                                {
                                    std::pair{0, 5},
                                    std::pair{0, 6}
                                }
                            },
                            true, true)) {
                        moves.emplace_back(0, 6);
                    }
                    // Black queen-side castling: squares d8, c8, and b8 must be empty and
                    // the rook must be at a8.
                    if (white_queen_side_castle && board[0][3].is_empty() &&
                        board[0][2].is_empty() && board[0][1].is_empty() &&
                        board[0][0] == Piece{PieceType::ROOK, false} &&
                        is_under_attack(std::vector{
                                            {
                                                std::pair{0, 3},
                                                std::pair{0, 2},
                                                std::pair{0, 1}
                                            }
                                        }, true, true)) {
                        moves.emplace_back(0, 2);
                    }
                }
            }
            break;
        }
        default:
            throw std::domain_error("Unknown piece type");
    }

    // ----------------------- Pinned Piece Verification -----------------------
    if (validate_pin) {
        int i{0};
        while (true) {
            std::pair<int, int> &move = moves[i];

            const Piece dest_piece{board[move.first][move.second]};
            make_move_raw(pos, move);
            if (in_check(board[move.first][move.second].isWhite)) {
                moves.erase(moves.begin() + i);
            } else {
                ++i;
            }
            make_move_raw(move, pos);
            board[move.first][move.second] = dest_piece;
            if (i >= moves.size()) {
                break;
            }
        }
    }

    return moves;
}

std::unordered_map<std::pair<int, int>, std::pair<Piece, std::vector<std::pair<
    int, int> > > > Board::get_all_valid_moves_raw(
    const bool &attack_moves_only, const bool &validate_pins) {
    std::unordered_map<std::pair<int, int>, std::pair<Piece, std::vector<
        std::pair<int, int> > > > moves{};
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            std::pair pos{i, j};
            moves[pos] = std::make_pair(get_piece(pos),
                                        get_valid_moves_raw(
                                            pos, attack_moves_only,
                                            validate_pins));
        }
    }
    return moves;
}

bool Board::is_under_attack(const std::pair<int, int> &pos,
                            const bool &white_is_attacking) {
    for (std::unordered_map attacking_moves{
             get_all_valid_moves_raw(true, false)
         }; const auto &[fst, snd]: attacking_moves | std::views::values) {
        if (fst.isWhite == white_is_attacking) {
            if (std::ranges::find_if(snd, [&pos](const std::pair<int, int> &p) {
                return p.first == pos.first && p.second == pos.second;
            }) != snd.end()) {
                return true;
            }
        }
    }

    return false;
}

bool Board::is_under_attack(const std::vector<std::pair<int, int> > &positions,
                            const bool &white_is_attacking, const bool &nor) {
    bool result = true;
    for (std::unordered_map attacking_moves{
             get_all_valid_moves_raw(true, false)
         }; const auto &[fst, snd]: attacking_moves | std::views::values) {
        if (fst.isWhite == white_is_attacking) {
            if (!nor) {
                for (const auto &pos: positions) {
                    if (std::ranges::find_if(
                            snd, [&pos](const std::pair<int, int> &p) {
                                return p.first == pos.first && p.second == pos.
                                       second;
                            }) == snd.end()) {
                        result = false;
                    }
                }
            } else {
                for (const auto &pos: positions) {
                    if (std::ranges::find_if(
                            snd, [&pos](const std::pair<int, int> &p) {
                                return p.first == pos.first && p.second == pos.
                                       second;
                            }) != snd.end()) {
                        result = false;
                    }
                }
            }
        }
    }

    return result;
}

bool Board::in_check(const bool &white) {
    return is_under_attack(find_piece(Piece{PieceType::PAWN, white})[0],
                           !white);
}
