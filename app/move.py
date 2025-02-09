from typing import Tuple, Optional, Type

class Move:
    """Represents a chess move from a source square to a target square.

    The default initialization uses board-index tuple coordinates:
      - source: a tuple (row, col) where row and col are integers (0-indexed)
      - target: a tuple (row, col) where row and col are integers (0-indexed)

    Alternatively, a Move can be created from algebraic notation using the
    `from_algebraic` class method.

    Attributes:
        source (Tuple[int, int]): The starting square as board indexes.
        target (Tuple[int, int]): The destination square as board indexes.
        promotion (Optional[str]): Optional piece type for pawn promotion
                                   (e.g., 'q' for queen).
    """

    def __init__(self, source: Tuple[int, int], target: Tuple[int, int],
                 promotion: Optional[str] = None) -> None:
        """Initializes a move using board-index tuples.

        Args:
            source (Tuple[int, int]): Starting square given as (row, col).
            target (Tuple[int, int]): Destination square given as (row, col).
            promotion (Optional[str]): Optional promotion piece type.
        """
        # Validate that both source and target positions are within the 8x8 board bounds.
        if not self._is_valid_position(source):
            raise ValueError(f"Invalid source position {source}. "
                             "Each coordinate must be between 0 and 7.")
        if not self._is_valid_position(target):
            raise ValueError(f"Invalid target position {target}. "
                             "Each coordinate must be between 0 and 7.")
        if promotion is not None and not isinstance(promotion, str) and promotion not in "qbnr":
            raise ValueError(f"Invalid promotion {promotion}. "
                             "Promotion must be either 'q', 'b', 'n' or 'r'.")

        self.source = source
        self.target = target
        self.promotion = promotion

    @classmethod
    def from_algebraic(cls: Type["Move"], source: str, target: str,
                       promotion: Optional[str] = None) -> "Move":
        """Creates a Move instance from algebraic notation.

        Converts coordinates from standard algebraic notation (e.g., "e2", "e4")
        into board-index tuples. The conversion assumes the board is indexed so that:
            - row index 0 corresponds to rank 8
            - row index 7 corresponds to rank 1

        Args:
            source (str): Starting square in algebraic notation (e.g., "e2").
            target (str): Destination square in algebraic notation (e.g., "e4").
            promotion (Optional[str]): Optional promotion piece type.

        Returns:
            Move: A new Move instance with converted board indexes.
        """
        src = cls._algebraic_to_index(source)
        dst = cls._algebraic_to_index(target)

        # Even though __init__ will re-check, perform an extra check here for clarity.
        if not cls._is_valid_position(src):
            raise ValueError(f"Algebraic source '{source}' converts to invalid board position {src}.")
        if not cls._is_valid_position(dst):
            raise ValueError(f"Algebraic target '{target}' converts to invalid board position {dst}.")

        return cls(src, dst, promotion)

    @staticmethod
    def _algebraic_to_index(square: str) -> Tuple[int, int]:
        """Converts an algebraic square notation to board indexes.

        Assumes that files are represented by letters 'a' to 'h' and ranks by
        digits '1' to '8'. The conversion produces a tuple (row, col) where row 0
        corresponds to rank 8.

        Args:
            square (str): Square in algebraic notation (e.g., "e2").

        Returns:
            Tuple[int, int]: The corresponding board indexes as (row, col).
        """
        if len(square) != 2:
            raise ValueError("Invalid square notation: must be exactly two characters (e.g., 'e2').")

        file_char, rank_char = square[0], square[1]
        if file_char.lower() not in 'abcdefgh' or rank_char not in '12345678':
            raise ValueError("Invalid square notation: file must be 'a'-'h' and rank must be '1'-'8'.")

        col = ord(file_char.lower()) - ord('a')
        row = 8 - int(rank_char)
        return row, col

    @staticmethod
    def _is_valid_position(pos: Tuple[int, int]) -> bool:
        """Checks if a board-index tuple is within the bounds of the chess board.

        A valid position is a tuple of two integers,
        where both integers are in the range 0 to 7 (inclusive).

        Args:
            pos (Tuple[int, int]): The board coordinates as (row, col).

        Returns:
            bool: True if pos is valid; False otherwise.
        """
        if not isinstance(pos, tuple) or len(pos) != 2:
            return False
        row, col = pos
        return isinstance(row, int) and isinstance(col, int) and 0 <= row < 8 and 0 <= col < 8

    def __repr__(self) -> str:
        """Returns an unambiguous string representation of the Move.

        Shows the source and target coordinates (as tuples) and the promotion if set.

        Returns:
            str: String representation of this move.
        """
        promo = f"={self.promotion}" if self.promotion else ""
        return f"<Move from {self.source} to {self.target}{promo}>"