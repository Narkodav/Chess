#include "Chess.h"

namespace Chess
{
    void Calculator::getWhitePawnMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        PawnMoveVariablePack pack = {
            PieceTypes::WHITE_PAWN,
            PieceTypes::WHITE_KNIGHT,
            PieceTypes::WHITE_BISHOP,
            PieceTypes::WHITE_ROOK,
            PieceTypes::WHITE_QUEEN,
            PieceTypes::WHITE_KING,

            PieceTypes::BLACK_PAWN,
            PieceTypes::BLACK_KNIGHT,
            PieceTypes::BLACK_BISHOP,
            PieceTypes::BLACK_ROOK,
            PieceTypes::BLACK_QUEEN,
            PieceTypes::BLACK_KING,

            Board::Flags::WHITE_CHECKED,
            Board::Flags::BLACK_CHECKED
        };

        getPawnMoves(board, nextBoards, pack, RANK_6,
            pawnMoveForward, pawnMoveForwardDouble, moveBackward,
            static_cast<uint64_t(*)(uint64_t, uint64_t)>(pawnAttackForwardLeft),
            static_cast<uint64_t(*)(uint64_t, uint64_t)>(pawnAttackForwardRight),
            moveRightBackward, moveLeftBackward, pawnShouldPromoteWhite,
            pawnWhiteCalculator, pawnBlackCalculator);
    }

    void Calculator::getBlackPawnMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        PawnMoveVariablePack pack = {
            PieceTypes::BLACK_PAWN,
            PieceTypes::BLACK_KNIGHT,
            PieceTypes::BLACK_BISHOP,
            PieceTypes::BLACK_ROOK,
            PieceTypes::BLACK_QUEEN,
            PieceTypes::BLACK_KING,

            PieceTypes::WHITE_PAWN,
            PieceTypes::WHITE_KNIGHT,
            PieceTypes::WHITE_BISHOP,
            PieceTypes::WHITE_ROOK,
            PieceTypes::WHITE_QUEEN,
            PieceTypes::WHITE_KING,

            Board::Flags::BLACK_CHECKED,
            Board::Flags::WHITE_CHECKED
        };

        getPawnMoves(board, nextBoards, pack, RANK_3,
            pawnMoveBackward, pawnMoveBackwardDouble, moveForward,
            static_cast<uint64_t(*)(uint64_t, uint64_t)>(pawnAttackBackwardRight),
            static_cast<uint64_t(*)(uint64_t, uint64_t)>(pawnAttackBackwardLeft),
            moveLeftForward, moveRightForward, pawnShouldPromoteBlack,
            pawnBlackCalculator, pawnWhiteCalculator);
    }
       
    void Calculator::getWhiteKnightMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        LookupTableMovePack pack = {
            PieceTypes::WHITE_KNIGHT,

            PieceTypes::WHITE_PAWN,
            PieceTypes::WHITE_KNIGHT,
            PieceTypes::WHITE_BISHOP,
            PieceTypes::WHITE_ROOK,
            PieceTypes::WHITE_QUEEN,
            PieceTypes::WHITE_KING,

            PieceTypes::BLACK_PAWN,
            PieceTypes::BLACK_KNIGHT,
            PieceTypes::BLACK_BISHOP,
            PieceTypes::BLACK_ROOK,
            PieceTypes::BLACK_QUEEN,
            PieceTypes::BLACK_KING,

            Board::Flags::WHITE_CHECKED,
            Board::Flags::BLACK_CHECKED,

            board.getBitBoard().getAllWhitePieces()
        };

        getKnightMoves(board, nextBoards, pack,
            pawnWhiteCalculator, pawnBlackCalculator);
    }

    void Calculator::getBlackKnightMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        LookupTableMovePack pack = {
            PieceTypes::BLACK_KNIGHT,

            PieceTypes::BLACK_PAWN,
            PieceTypes::BLACK_KNIGHT,
            PieceTypes::BLACK_BISHOP,
            PieceTypes::BLACK_ROOK,
            PieceTypes::BLACK_QUEEN,
            PieceTypes::BLACK_KING,

            PieceTypes::WHITE_PAWN,
            PieceTypes::WHITE_KNIGHT,
            PieceTypes::WHITE_BISHOP,
            PieceTypes::WHITE_ROOK,
            PieceTypes::WHITE_QUEEN,
            PieceTypes::WHITE_KING,

            Board::Flags::BLACK_CHECKED,
            Board::Flags::WHITE_CHECKED,

            board.getBitBoard().getAllBlackPieces()
        };

        getKnightMoves(board, nextBoards, pack,
            pawnBlackCalculator, pawnWhiteCalculator);
    }

    void Calculator::getWhiteBishopMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        LookupTableMovePack pack = {
            PieceTypes::WHITE_BISHOP,

            PieceTypes::WHITE_PAWN,
            PieceTypes::WHITE_KNIGHT,
            PieceTypes::WHITE_BISHOP,
            PieceTypes::WHITE_ROOK,
            PieceTypes::WHITE_QUEEN,
            PieceTypes::WHITE_KING,

            PieceTypes::BLACK_PAWN,
            PieceTypes::BLACK_KNIGHT,
            PieceTypes::BLACK_BISHOP,
            PieceTypes::BLACK_ROOK,
            PieceTypes::BLACK_QUEEN,
            PieceTypes::BLACK_KING,

            Board::Flags::WHITE_CHECKED,
            Board::Flags::BLACK_CHECKED,

            board.getBitBoard().getAllWhitePieces()
        };

        getSlidingMoves(board, nextBoards, pack,
            bishopLookupFunction, pawnWhiteCalculator, pawnBlackCalculator);
    }

    void Calculator::getBlackBishopMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        LookupTableMovePack pack = {
            PieceTypes::BLACK_BISHOP,

            PieceTypes::BLACK_PAWN,
            PieceTypes::BLACK_KNIGHT,
            PieceTypes::BLACK_BISHOP,
            PieceTypes::BLACK_ROOK,
            PieceTypes::BLACK_QUEEN,
            PieceTypes::BLACK_KING,

            PieceTypes::WHITE_PAWN,
            PieceTypes::WHITE_KNIGHT,
            PieceTypes::WHITE_BISHOP,
            PieceTypes::WHITE_ROOK,
            PieceTypes::WHITE_QUEEN,
            PieceTypes::WHITE_KING,

            Board::Flags::BLACK_CHECKED,
            Board::Flags::WHITE_CHECKED,

            board.getBitBoard().getAllBlackPieces()
        };

        getSlidingMoves(board, nextBoards, pack,
            bishopLookupFunction, pawnBlackCalculator, pawnWhiteCalculator);
    }

    void Calculator::getWhiteRookMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        LookupTableMovePack pack = {
            PieceTypes::WHITE_ROOK,

            PieceTypes::WHITE_PAWN,
            PieceTypes::WHITE_KNIGHT,
            PieceTypes::WHITE_BISHOP,
            PieceTypes::WHITE_ROOK,
            PieceTypes::WHITE_QUEEN,
            PieceTypes::WHITE_KING,

            PieceTypes::BLACK_PAWN,
            PieceTypes::BLACK_KNIGHT,
            PieceTypes::BLACK_BISHOP,
            PieceTypes::BLACK_ROOK,
            PieceTypes::BLACK_QUEEN,
            PieceTypes::BLACK_KING,

            Board::Flags::WHITE_CHECKED,
            Board::Flags::BLACK_CHECKED,

            board.getBitBoard().getAllWhitePieces()
        };

        getRookMoves(board, nextBoards, pack, WHITE_ROOK_KINGSIDE_START, WHITE_ROOK_QUEENSIDE_START,
            Board::Flags::WHITE_HAS_CASTLING_KINGSIDE_RIGHTS, Board::Flags::WHITE_HAS_CASTLING_QUEENSIDE_RIGHTS,
            rookLookupFunction, pawnWhiteCalculator, pawnBlackCalculator);
    }

    void Calculator::getBlackRookMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        LookupTableMovePack pack = {
            PieceTypes::BLACK_ROOK,

            PieceTypes::BLACK_PAWN,
            PieceTypes::BLACK_KNIGHT,
            PieceTypes::BLACK_BISHOP,
            PieceTypes::BLACK_ROOK,
            PieceTypes::BLACK_QUEEN,
            PieceTypes::BLACK_KING,

            PieceTypes::WHITE_PAWN,
            PieceTypes::WHITE_KNIGHT,
            PieceTypes::WHITE_BISHOP,
            PieceTypes::WHITE_ROOK,
            PieceTypes::WHITE_QUEEN,
            PieceTypes::WHITE_KING,

            Board::Flags::BLACK_CHECKED,
            Board::Flags::WHITE_CHECKED,

            board.getBitBoard().getAllBlackPieces()
        };

        getRookMoves(board, nextBoards, pack, BLACK_ROOK_KINGSIDE_START, BLACK_ROOK_QUEENSIDE_START,
            Board::Flags::BLACK_HAS_CASTLING_KINGSIDE_RIGHTS, Board::Flags::BLACK_HAS_CASTLING_QUEENSIDE_RIGHTS,
            rookLookupFunction, pawnBlackCalculator, pawnWhiteCalculator);
    }

    void Calculator::getWhiteQueenMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        LookupTableMovePack pack = {
            PieceTypes::WHITE_QUEEN,

            PieceTypes::WHITE_PAWN,
            PieceTypes::WHITE_KNIGHT,
            PieceTypes::WHITE_BISHOP,
            PieceTypes::WHITE_ROOK,
            PieceTypes::WHITE_QUEEN,
            PieceTypes::WHITE_KING,

            PieceTypes::BLACK_PAWN,
            PieceTypes::BLACK_KNIGHT,
            PieceTypes::BLACK_BISHOP,
            PieceTypes::BLACK_ROOK,
            PieceTypes::BLACK_QUEEN,
            PieceTypes::BLACK_KING,

            Board::Flags::WHITE_CHECKED,
            Board::Flags::BLACK_CHECKED,

            board.getBitBoard().getAllWhitePieces()
        };

        getSlidingMoves(board, nextBoards, pack,
            queenLookupFunction, pawnWhiteCalculator, pawnBlackCalculator);
    }

    void Calculator::getBlackQueenMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        LookupTableMovePack pack = {
            PieceTypes::BLACK_QUEEN,

            PieceTypes::BLACK_PAWN,
            PieceTypes::BLACK_KNIGHT,
            PieceTypes::BLACK_BISHOP,
            PieceTypes::BLACK_ROOK,
            PieceTypes::BLACK_QUEEN,
            PieceTypes::BLACK_KING,

            PieceTypes::WHITE_PAWN,
            PieceTypes::WHITE_KNIGHT,
            PieceTypes::WHITE_BISHOP,
            PieceTypes::WHITE_ROOK,
            PieceTypes::WHITE_QUEEN,
            PieceTypes::WHITE_KING,

            Board::Flags::BLACK_CHECKED,
            Board::Flags::WHITE_CHECKED,

            board.getBitBoard().getAllBlackPieces()
        };

        getSlidingMoves(board, nextBoards, pack,
            queenLookupFunction, pawnBlackCalculator, pawnWhiteCalculator);
    }

    void Calculator::getWhiteKingMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        KingMovePack pack = {
            PieceTypes::WHITE_PAWN,
            PieceTypes::WHITE_KNIGHT,
            PieceTypes::WHITE_BISHOP,
            PieceTypes::WHITE_ROOK,
            PieceTypes::WHITE_QUEEN,
            PieceTypes::WHITE_KING,

            PieceTypes::BLACK_PAWN,
            PieceTypes::BLACK_KNIGHT,
            PieceTypes::BLACK_BISHOP,
            PieceTypes::BLACK_ROOK,
            PieceTypes::BLACK_QUEEN,
            PieceTypes::BLACK_KING,

            Board::Flags::WHITE_CHECKED,
            Board::Flags::BLACK_CHECKED,

            Board::Flags::WHITE_HAS_CASTLING_QUEENSIDE_RIGHTS,
            Board::Flags::WHITE_HAS_CASTLING_KINGSIDE_RIGHTS,

            WHITE_KINGSIDE_CASTLING_PATH,
            WHITE_QUEENSIDE_CASTLING_PATH,

            WHITE_KINGSIDE_CASTLING_KING_END,
            WHITE_QUEENSIDE_CASTLING_KING_END,

            WHITE_ROOK_KINGSIDE_START,
            WHITE_ROOK_QUEENSIDE_START,

            WHITE_KINGSIDE_CASTLING_ROOK_END,
            WHITE_QUEENSIDE_CASTLING_ROOK_END,

            board.getBitBoard().getAllWhitePieces()
        };

        getKingMoves(board, nextBoards, pack,
            pawnWhiteCalculator, pawnBlackCalculator);
    }

    void Calculator::getBlackKingMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        KingMovePack pack = {
            PieceTypes::BLACK_PAWN,
            PieceTypes::BLACK_KNIGHT,
            PieceTypes::BLACK_BISHOP,
            PieceTypes::BLACK_ROOK,
            PieceTypes::BLACK_QUEEN,
            PieceTypes::BLACK_KING,

            PieceTypes::WHITE_PAWN,
            PieceTypes::WHITE_KNIGHT,
            PieceTypes::WHITE_BISHOP,
            PieceTypes::WHITE_ROOK,
            PieceTypes::WHITE_QUEEN,
            PieceTypes::WHITE_KING,

            Board::Flags::BLACK_CHECKED,
            Board::Flags::WHITE_CHECKED,

            Board::Flags::BLACK_HAS_CASTLING_QUEENSIDE_RIGHTS,
            Board::Flags::BLACK_HAS_CASTLING_KINGSIDE_RIGHTS,

            BLACK_KINGSIDE_CASTLING_PATH,
            BLACK_QUEENSIDE_CASTLING_PATH,

            BLACK_KINGSIDE_CASTLING_KING_END,
            BLACK_QUEENSIDE_CASTLING_KING_END,

            BLACK_ROOK_KINGSIDE_START,
            BLACK_ROOK_QUEENSIDE_START,

            BLACK_KINGSIDE_CASTLING_ROOK_END,
            BLACK_QUEENSIDE_CASTLING_ROOK_END,

            board.getBitBoard().getAllBlackPieces()
        };

        getKingMoves(board, nextBoards, pack,
            pawnBlackCalculator, pawnWhiteCalculator);
    }

};