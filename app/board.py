import copy
import pprint
from typing import Optional, List, Dict, Final, Tuple

from app.move import Move


class Board:
    """A class representing a chess board with Unicode piece representations.

    Attributes:
        CHESS_SYMBOLS: Mapping of ASCII piece characters to Unicode symbols.
        board: 2D list representing the chess board state.
        turn: Current player's turn ('white' or 'black').
        castling: Dictionary tracking castling availability.
        en_passant: Optional square where en passant capture is possible.
        halfmove_clock: Number of moves since last pawn advance or piece capture.
        fullmove_number: Number of complete turns in the game.
    """

    CHESS_SYMBOLS: Final[Dict[str, str]] = {
        'R': '♜', 'N': '♞', 'B': '♝', 'Q': '♛', 'K': '♚', 'P': '♟',
        'r': '♖', 'n': '♘', 'b': '♗', 'q': '♕', 'k': '♔', 'p': '♙',
        '.': '·'
    }

    def __init__(self, position: Optional[List[List[str]]] = None) -> None:
        """Initialize a new chess board.

        Args:
            position: Optional 2D list representing initial board state.
                     If None, uses standard chess starting position.
        """
        self.board: List[List[str]] = position or self._default_board()
        self.white_turn: bool = True
        self.castling: Dict[str, bool] = {'K': True, 'Q': True, 'k': True,
                                          'q': True}
        self.white_in_check: bool = False
        self.black_in_check: bool = False
        self.en_passant: Optional[Tuple[int, int]] = None
        self.halfmove_clock: int = 0
        self.fullmove_number: int = 1

    @staticmethod
    def _default_board() -> List[List[str]]:
        """Create a new board with standard starting position.

        Returns:
            List[List[str]]: 2D list representing the initial chess position.
        """
        return [
            ['r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'],
            ['p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'],
            ['.'] * 8,
            ['.'] * 8,
            ['.'] * 8,
            ['.'] * 8,
            ['P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'],
            ['R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R']
        ]

    def __str__(self) -> str:
        """Generate Unicode representation of the board with coordinates.

        Returns:
            str: Formatted string showing current board state with ranks and files.
        """
        border: str = '  ' + '─' * 17
        ranks: List[str] = [border]

        for i, row in enumerate(self.board):
            rank_num: int = 8 - i
            symbols: List[str] = [self.CHESS_SYMBOLS[cell] for cell in row]
            ranks.append(
                f'{rank_num} │ {" ".join(symbols)} │'
            )

        ranks.append(border)
        ranks.append('    a b c d e f g h')
        return '\n'.join(ranks)

    def __repr__(self) -> str:
        """Generate a detailed representation of the board state.

        Returns:
            str: String containing complete board state including position,
                 turn, castling rights, en passant, and move counts.
        """
        board_str = ''.join(''.join(row) for row in self.board)
        return (f"<Board position='{board_str}' "
                f"turn='{"white" if self.white_turn else "black"}' "
                f"castling={self.castling} "
                f"en_passant={self.en_passant} "
                f"halfmove={self.halfmove_clock} "
                f"fullmove={self.fullmove_number}>")

    def get(self, rank: int, file: str) -> str:
        """Get piece at specified position using chess coordinates.

        Args:
            rank: Integer from 1 to 8 representing the rank.
            file: Single character from 'a' to 'h' representing the file.

        Returns:
            str: Character representing the piece at the position.
        """
        return self.board[8 - rank][ord(file) - ord('a')]

    def make_raw_move(self, move: Move) -> None:
        """Updates the board state by executing the given move.

        This method does not validate the move's legality - it simply updates
        the board state by moving the piece from source to target square.

        Args:
            move: Move object containing source and target coordinates
        """
        # Extract source and target coordinates
        src_row, src_col = move.source
        dst_row, dst_col = move.target

        # Get the piece at the source square
        piece = self.board[src_row][src_col]

        # Move the piece to the target square
        self.board[src_row][src_col] = '.'
        self.board[dst_row][dst_col] = piece

    def make_move(self, move: Move) -> None:

        # Extract source and target coordinates
        src_row, src_col = move.source
        dst_row, dst_col = move.target

        # Checking if move is valid
        valid_moves = self.get_valid_moves(move.source)
        if move.target not in valid_moves or self.board[src_row][
            src_col] == '.' or self.board[src_row][
            src_col].isupper() != self.white_turn:
            raise ValueError(f"Invalid move: {move}. ")

        # Setting en passant target
        pawn_start_row = 6 if self.white_turn else 1
        pawn_two_move_row = 4 if self.white_turn else 3
        if self.board[src_row][
            src_col].upper() == 'P' and src_row == pawn_start_row and dst_row == pawn_two_move_row:
            self.en_passant = (
                src_row - 1 if self.white_turn else src_row + 1, src_col)
        else:
            self.en_passant = None

        # Verifying castling rights
        king_pos = self.find('K')[0] if self.white_turn else self.find('k')[0]
        if king_pos == move.source:
            self.castling['K' if self.white_turn else 'k'] = False
            self.castling['Q' if self.white_turn else 'q'] = False

        queen_rook_pos = (7, 0) if self.white_turn else (0, 0)
        if self.board[src_row][
            src_col].upper() == 'R' and move.source == queen_rook_pos:
            self.castling['Q' if self.white_turn else 'q'] = False

        king_rook_pos = (7, 7) if self.white_turn else (0, 7)
        if self.board[src_row][
            src_col].upper() == 'R' and move.source == king_rook_pos:
            self.castling['K' if self.white_turn else 'k'] = False

        # Applying the move
        self.make_raw_move(move)

        # Player just moved so they must not be in check
        if self.white_turn:
            self.white_in_check = False
        else:
            self.black_in_check = False

        # Set the half move clock
        if self.board[src_row][src_col].upper() == 'P' or \
                self.board[dst_row][dst_col].upper() != '.':
            self.halfmove_clock = 0
        else:
            self.halfmove_clock += 1

        # Set the full move number
        if not self.white_turn:
            self.fullmove_number += 1

        # Change the player's turn
        self.white_turn = not self.white_turn

        # Check if the new player is in check
        if self.white_turn:
            self.white_in_check = self.in_check(self.white_turn)
        else:
            self.black_in_check = self.in_check(self.white_turn)

        # TODO: Checkmate Check

        # TODO: Stalemate Check


# TODO filer valid moves if in check
    def get_valid_moves(
            self, pos: Tuple[int, int],
            attack_moves_only: bool = False,
            validate_pin: bool = True
    ) -> List[Tuple[int, int]]:
        """
        Given a piece's current board coordinates (row, col), return a list of all valid moves
        as (row, col) tuples. The coordinate system is 0-indexed: row 0 is the top of the board,
        row 7 the bottom. Moves generated here are pseudo-legal (they follow piece movement and
        simple castling/en passant rules, but do not consider check conditions).

        Args:
            pos (Tuple[int, int]): The (row, col) coordinate of the piece.
            attack_moves_only (bool): If True, only return moves that are allowed
                to capture other pieces (if a piece is present, otherwise, if the
                square is empty still return). i.e. Forward pawn moves and
                castling moves will not be returned if True.
            validate_pin (bool): If True, filter out moves that would put the current player in check.

        Returns:
            List[Tuple[int, int]]: List of destination (row, col) coordinates.
        """
        moves: List[Tuple[int, int]] = []
        row, col = pos
        piece = self.board[row][col]
        if piece == '.':
            return moves  # no piece at this position

        # Determine side: white pieces are uppercase, black are lowercase.
        is_white = piece.isupper()

        # Helper function to verify a coordinate is on the board.
        def is_inside(r: int, c: int) -> bool:
            return 0 <= r < 8 and 0 <= c < 8

        # ----------------------- Pawn Moves -----------------------
        if piece.upper() == 'P':
            # For white pawns, they move upward (i.e. decreasing row index);
            # for black pawns, they move downward.
            direction = -1 if is_white else 1
            start_row = 6 if is_white else 1

            if not attack_moves_only:
                # Move forward one.
                forward = row + direction
                if is_inside(forward, col) and self.board[forward][col] == '.':
                    moves.append((forward, col))
                    # If on starting rank, pawn can move two squares forward.
                    if row == start_row:
                        forward2 = row + 2 * direction
                        if is_inside(forward2, col) and self.board[forward2][
                            col] == '.':
                            moves.append((forward2, col))

            # Captures (diagonally).
            for dc in [-1, 1]:
                new_r, new_c = row + direction, col + dc
                if is_inside(new_r, new_c):
                    target = self.board[new_r][new_c]
                    if target != '.' and (target.isupper() != is_white):
                        moves.append((new_r, new_c))
                    # Check en passant: if en_passant is set and matches the candidate square.
                    if self.en_passant:
                        if new_r == self.en_passant[0] and new_c == \
                                self.en_passant[1]:
                            moves.append((new_r, new_c))

        # ----------------------- Knight Moves -----------------------
        elif piece.upper() == 'N':
            knight_offsets = [(2, 1), (2, -1), (-2, 1), (-2, -1),
                              (1, 2), (1, -2), (-1, 2), (-1, -2)]
            for dr, dc in knight_offsets:
                new_r, new_c = row + dr, col + dc
                if is_inside(new_r, new_c):
                    target = self.board[new_r][new_c]
                    if target == '.' or (target.isupper() != is_white):
                        moves.append((new_r, new_c))

        # ----------------------- Rook Moves -----------------------
        elif piece.upper() == 'R':
            directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]
            for dr, dc in directions:
                r, c = row, col
                while True:
                    r += dr
                    c += dc
                    if not is_inside(r, c):
                        break
                    target = self.board[r][c]
                    if target == '.':
                        moves.append((r, c))
                    elif target.isupper() != is_white:
                        moves.append((r, c))
                        break
                    else:
                        break

        # ----------------------- Bishop Moves -----------------------
        elif piece.upper() == 'B':
            directions = [(-1, -1), (-1, 1), (1, -1), (1, 1)]
            for dr, dc in directions:
                r, c = row, col
                while True:
                    r += dr
                    c += dc
                    if not is_inside(r, c):
                        break
                    target = self.board[r][c]
                    if target == '.':
                        moves.append((r, c))
                    elif target.isupper() != is_white:
                        moves.append((r, c))
                        break
                    else:
                        break

        # ----------------------- Queen Moves -----------------------
        elif piece.upper() == 'Q':
            directions = [(-1, 0), (1, 0), (0, -1), (0, 1),
                          (-1, -1), (-1, 1), (1, -1), (1, 1)]
            for dr, dc in directions:
                r, c = row, col
                while True:
                    r += dr
                    c += dc
                    if not is_inside(r, c):
                        break
                    target = self.board[r][c]
                    if target == '.':
                        moves.append((r, c))
                    elif target.isupper() != is_white:
                        moves.append((r, c))
                        break
                    else:
                        break

        # ----------------------- King Moves (including castling) -----------------------
        elif piece.upper() == 'K':
            king_offsets = [(-1, -1), (-1, 0), (-1, 1),
                            (0, -1), (0, 1),
                            (1, -1), (1, 0), (1, 1)]
            for dr, dc in king_offsets:
                new_r, new_c = row + dr, col + dc
                if is_inside(new_r, new_c):
                    target = self.board[new_r][new_c]
                    if target == '.' or (target.isupper() != is_white):
                        moves.append((new_r, new_c))

            if not attack_moves_only:
                # For castling, we check that the king is at its starting square.
                if is_white and row == 7 and col == 4:
                    # White kingside castling: squares f1 and g1 must be empty
                    # and the rook must be at h1.
                    if (
                            self.castling.get('K', False) and
                            self.board[7][5] == '.' and
                            self.board[7][6] == '.' and
                            self.board[7][7] == 'R' and
                            not self.is_under_attack((7, 5), False) and
                            not self.is_under_attack((7, 6), False)
                    ):
                        moves.append((7, 6))
                    # White queenside castling: squares d1, c1, and b1 must be empty
                    # and the rook must be at a1.
                    if (
                            self.castling.get('Q', False) and
                            self.board[7][3] == '.' and
                            self.board[7][2] == '.' and
                            self.board[7][1] == '.' and
                            self.board[7][0] == 'R' and
                            not self.is_under_attack((7, 3), False) and
                            not self.is_under_attack((7, 2), False) and
                            not self.is_under_attack((7, 1), False)
                    ):
                        moves.append((7, 2))
                elif not is_white and row == 0 and col == 4:
                    # Black kingside castling: squares f8 and g8 must be empty and
                    # the rook must be at h8.
                    if (
                            self.castling.get('k', False) and
                            self.board[0][5] == '.' and
                            self.board[0][6] == '.' and
                            self.board[0][7] == 'r' and
                            not self.is_under_attack((0, 5), True) and
                            not self.is_under_attack((0, 6), True)
                    ):
                        moves.append((0, 6))
                    # Black queenside castling: squares d8, c8, and b8 must be empty and
                    # the rook must be at a8.
                    if (
                            self.castling.get('q', False) and
                            self.board[0][3] == '.' and
                            self.board[0][2] == '.' and
                            self.board[0][1] == '.' and
                            self.board[0][0] == 'r' and
                            not self.is_under_attack((0, 3), True) and
                            not self.is_under_attack((0, 2), True) and
                            not self.is_under_attack((0, 1), True)
                    ):
                        moves.append((0, 2))

        # ----------------------- Pinned Piece Verification -----------------------
        if validate_pin:
            for move in moves:
                self.make_raw_move(Move(pos, move))
                if self.in_check(self.board[row][col].isupper()):
                    moves.remove(move)
                self.make_raw_move(Move(move, pos))

        return moves

    def get_all_valid_moves(self, attack_moves_only: bool = False,
                            validate_pins: bool = True) -> Dict[
        Tuple[int, int], Tuple[str, List[Tuple[int, int]]]]:
        moves = {}
        for i in range(8):
            for j in range(8):
                moves[i, j] = (
                    self.board[i][j],
                    self.get_valid_moves(
                        (i, j),
                        attack_moves_only=attack_moves_only,
                        validate_pin=validate_pins)
                )
        return moves

    def find(self, piece: str) -> List[Tuple[int, int]]:
        positions = []
        for i in range(8):
            for j in range(8):
                if self.board[i][j] == piece:
                    positions.append((i, j))

        return positions

    def is_under_attack(self, pos: Tuple[int, int],
                        white_is_attacking: bool) -> bool:
        attacking_moves = self.get_all_valid_moves(attack_moves_only=True,
                                                   validate_pins=False)
        for _, v in attacking_moves.items():
            if v[0].isupper() == white_is_attacking:
                if pos in v[1]:
                    return True
        return False

    def in_check(self, white: bool) -> bool:
        piece = "K" if white else "k"
        return self.is_under_attack(self.find(piece)[0], not white)


if __name__ == '__main__':
    board = Board()
    pprint.pprint(board.get_all_valid_moves())
    # board.make_raw_move(Move.from_algebraic('e2', 'e4'))
    print(board)
