#pragma once
#include "Utilities/ArrayNd.h"

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <bit>

#include "Constants.h"
#include "MagicBishops.h"
#include "MagicRooks.h"

namespace Chess
{
    enum class PieceTypes : uint8_t
    {
        EMPTY,
        WHITE_PAWN,
        WHITE_KNIGHT,
        WHITE_BISHOP,
        WHITE_ROOK,
        WHITE_QUEEN,
        WHITE_KING,

        BLACK_PAWN,
        BLACK_KNIGHT,
        BLACK_BISHOP,
        BLACK_ROOK,
        BLACK_QUEEN,
        BLACK_KING,

        NUM
    };

    struct MagicInit {
        static MagicInit& getInstance() {
            static MagicInit instance;
            return instance;
        }
    private:
        MagicInit() {
            MagicBishops::initializeMagics();
            MagicRooks::initializeMagics();
            MagicBishops::debugMagicNumber(4);
            MagicRooks::debugMagicNumber(4);
        }
    };

    static inline MagicInit magicInit = MagicInit::getInstance();

    class Board
    {
    public:
        struct BitBoard {
            // One 64-bit integer for each piece type and color
            uint64_t whitePawns;
            uint64_t whiteKnights;
            uint64_t whiteBishops;
            uint64_t whiteRooks;
            uint64_t whiteQueens;
            uint64_t whiteKing;

            uint64_t blackPawns;
            uint64_t blackKnights;
            uint64_t blackBishops;
            uint64_t blackRooks;
            uint64_t blackQueens;
            uint64_t blackKing;

            // Get all white pieces
            inline uint64_t getAllWhitePieces() const
            {
                return whitePawns | whiteKnights | whiteBishops |
                    whiteRooks | whiteQueens | whiteKing;
            }

            inline uint64_t getAllBlackPieces() const
            {
                return blackPawns | blackKnights | blackBishops |
                    blackRooks | blackQueens | blackKing;
            }
            
            inline uint64_t getAllPieces() const
            {
                return getAllWhitePieces() | getAllBlackPieces();
            }

            static bool isOccupied(int rank, int file, uint64_t pieces)
            {
                return (pieces & (1ULL << rank * 8 + file)) != 0;
            }

            //// Generate pawn moves (shift by 8 for one square forward)
            //uint64_t pawnMoves = (whitePawns << 8) & ~allPieces; // & with empty squares

            //// Generate knight moves using pre-calculated lookup tables
            //uint64_t knightMoves = KnightMovesTable[fromSquare] & ~allWhitePieces;
        };

        enum class Flags : uint8_t
        {
            WHITE_CHECKED = 1 << 0,
            BLACK_CHECKED = 1 << 1,
            WHITE_HAS_CASTLING_KINGSIDE_RIGHTS = 1 << 2,
            WHITE_HAS_CASTLING_QUEENSIDE_RIGHTS = 1 << 3,
            BLACK_HAS_CASTLING_KINGSIDE_RIGHTS = 1 << 4,
            BLACK_HAS_CASTLING_QUEENSIDE_RIGHTS = 1 << 5,
        };

    private:
        BitBoard m_bitBoard;
        uint8_t m_enPassantSquare = 0; //square on which an en passant capture is possible
        uint8_t m_flags = 0;

    public:
        inline const BitBoard& getBitBoard() const { return m_bitBoard; };
        inline BitBoard& getBitBoard() { return m_bitBoard; };

        inline const uint8_t& getEnPassantSquare() const { return m_enPassantSquare; };
        inline uint8_t& getEnPassantSquare() { return m_enPassantSquare; };

        inline const uint8_t& getFlags() const { return m_flags; };
        inline uint8_t& getFlags() { return m_flags; };

        bool isWhiteChecked() const { return m_flags & static_cast<uint8_t>(Flags::WHITE_CHECKED); };
        bool isBlackChecked() const { return m_flags & static_cast<uint8_t>(Flags::BLACK_CHECKED); };

        Board() : m_bitBoard({ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }),
            m_enPassantSquare(0), m_flags(0) {};

        Board(const Board&) = default;
        Board& operator=(const Board&) = default;

        Board(Board&&) = default;
        Board& operator=(Board&&) = default;

        void reset()
        {
            // Reset white pieces
            m_bitBoard.whitePawns = 0x000000000000FF00ULL;    // Rank 2
            m_bitBoard.whiteKnights = 0x0000000000000042ULL;    // b1, g1
            m_bitBoard.whiteBishops = 0x0000000000000024ULL;    // c1, f1
            m_bitBoard.whiteRooks = 0x0000000000000081ULL;    // a1, h1
            m_bitBoard.whiteQueens = 0x0000000000000008ULL;    // d1
            m_bitBoard.whiteKing = 0x0000000000000010ULL;    // e1

            // Reset black pieces
            m_bitBoard.blackPawns = 0x00FF000000000000ULL;    // Rank 7
            m_bitBoard.blackKnights = 0x4200000000000000ULL;    // b8, g8
            m_bitBoard.blackBishops = 0x2400000000000000ULL;    // c8, f8
            m_bitBoard.blackRooks = 0x8100000000000000ULL;    // a8, h8
            m_bitBoard.blackQueens = 0x0800000000000000ULL;    // d8
            m_bitBoard.blackKing = 0x1000000000000000ULL;    // e8

            m_flags |= static_cast<uint8_t>(Flags::WHITE_HAS_CASTLING_KINGSIDE_RIGHTS);
            m_flags |= static_cast<uint8_t>(Flags::WHITE_HAS_CASTLING_QUEENSIDE_RIGHTS);
            m_flags |= static_cast<uint8_t>(Flags::BLACK_HAS_CASTLING_KINGSIDE_RIGHTS);
            m_flags |= static_cast<uint8_t>(Flags::BLACK_HAS_CASTLING_QUEENSIDE_RIGHTS);
                
            // Reset en passant square
            m_enPassantSquare = 0;
        }

        PieceTypes getPieceTypeAtSquare(int square) const {
            uint64_t squareMask = 1ULL << square;
            if (~m_bitBoard.getAllPieces() & squareMask)
                return PieceTypes::EMPTY;

            // White pieces
            if (m_bitBoard.whitePawns & squareMask)
                return PieceTypes::WHITE_PAWN;
            if (m_bitBoard.whiteKnights & squareMask)
                return PieceTypes::WHITE_KNIGHT;
            if (m_bitBoard.whiteBishops & squareMask)
                return PieceTypes::WHITE_BISHOP;
            if (m_bitBoard.whiteRooks & squareMask)
                return PieceTypes::WHITE_ROOK;
            if (m_bitBoard.whiteQueens & squareMask)
                return PieceTypes::WHITE_QUEEN;
            if (m_bitBoard.whiteKing & squareMask)
                return PieceTypes::WHITE_KING;

            // Black pieces
            if (m_bitBoard.blackPawns & squareMask)
                return PieceTypes::BLACK_PAWN;
            if (m_bitBoard.blackKnights & squareMask)
                return PieceTypes::BLACK_KNIGHT;
            if (m_bitBoard.blackBishops & squareMask)
                return PieceTypes::BLACK_BISHOP;
            if (m_bitBoard.blackRooks & squareMask)
                return PieceTypes::BLACK_ROOK;
            if (m_bitBoard.blackQueens & squareMask)
                return PieceTypes::BLACK_QUEEN;
            if (m_bitBoard.blackKing & squareMask)
                return PieceTypes::BLACK_KING;

            return PieceTypes::EMPTY;
        }
    };

    class Calculator
    {
    public:

        static std::vector<Board> getNextBoardsWhite(const Board& currentBoard) {

            std::vector<Board> nextBoards;
            nextBoards.reserve(MAXIMUM_CONSERVATIVE_MOVE_AMOUNT);
            getWhitePawnsMoves(currentBoard, nextBoards);
            getWhiteKnightsMoves(currentBoard, nextBoards);
            getWhiteBishopsMoves(currentBoard, nextBoards);
            getWhiteRooksMoves(currentBoard, nextBoards);
            getWhiteQueensMoves(currentBoard, nextBoards);
            getWhiteKingMoves(currentBoard, nextBoards);
            return nextBoards;
        }

        static std::vector<Board> getNextBoardsBlack(const Board& currentBoard) {

            std::vector<Board> nextBoards;
            nextBoards.reserve(MAXIMUM_CONSERVATIVE_MOVE_AMOUNT);
            getBlackPawnsMoves(currentBoard, nextBoards);
            getBlackKnightsMoves(currentBoard, nextBoards);
            getBlackBishopsMoves(currentBoard, nextBoards);
            getBlackRooksMoves(currentBoard, nextBoards);
            getBlackQueensMoves(currentBoard, nextBoards);
            getBlackKingMoves(currentBoard, nextBoards);
            return nextBoards;
        }

        static std::unordered_multimap<int, Board> getNextBoardsWhiteMultimap(const Board& currentBoard) {

            std::vector<Board> nextBoards;
            nextBoards.reserve(MAXIMUM_CONSERVATIVE_MOVE_AMOUNT);
            getWhitePawnsMoves(currentBoard, nextBoards);
            getWhiteKnightsMoves(currentBoard, nextBoards);
            getWhiteBishopsMoves(currentBoard, nextBoards);
            getWhiteRooksMoves(currentBoard, nextBoards);
            getWhiteQueensMoves(currentBoard, nextBoards);
            getWhiteKingMoves(currentBoard, nextBoards);

            std::unordered_multimap<int, Board> nextMap;

            for (auto& nextBoard : nextBoards)
            {
                int from;
                if (nextBoard.getBitBoard().whiteKing != currentBoard.getBitBoard().whiteKing)
                    from = Chess::Calculator::getFromToPair(
                        nextBoard.getBitBoard().whiteKing, currentBoard.getBitBoard().whiteKing).first;
                else if (nextBoard.getBitBoard().whitePawns != currentBoard.getBitBoard().whitePawns)
                    from = Chess::Calculator::getFromToPair(
                        nextBoard.getBitBoard().whitePawns, currentBoard.getBitBoard().whitePawns).first;
                else from = Chess::Calculator::getFromToPairWhite(nextBoard, currentBoard).first;
                nextMap.insert(std::make_pair(from, std::move(nextBoard)));
            }

            return nextMap;
        }

        static  std::unordered_multimap<int, Board> getNextBoardsBlackMultimap(const Board& currentBoard) {

            std::vector<Board> nextBoards;
            nextBoards.reserve(MAXIMUM_CONSERVATIVE_MOVE_AMOUNT);
            getBlackPawnsMoves(currentBoard, nextBoards);
            getBlackKnightsMoves(currentBoard, nextBoards);
            getBlackBishopsMoves(currentBoard, nextBoards);
            getBlackRooksMoves(currentBoard, nextBoards);
            getBlackQueensMoves(currentBoard, nextBoards);
            getBlackKingMoves(currentBoard, nextBoards);

            std::unordered_multimap<int, Board> nextMap;

            for (auto& nextBoard : nextBoards)
            {
                int from;
                if (nextBoard.getBitBoard().blackKing != currentBoard.getBitBoard().blackKing)
                    from = Chess::Calculator::getFromToPair(
                        nextBoard.getBitBoard().blackKing, currentBoard.getBitBoard().blackKing).first;
                else if (nextBoard.getBitBoard().blackPawns != currentBoard.getBitBoard().blackPawns)
                    from = Chess::Calculator::getFromToPair(
                        nextBoard.getBitBoard().blackPawns, currentBoard.getBitBoard().blackPawns).first;
                else from = Chess::Calculator::getFromToPairBlack(nextBoard, currentBoard).first;
                nextMap.insert(std::make_pair(from, std::move(nextBoard)));
            }

            return nextMap;
        }

        static bool isSquareUnderAttackWhite(const Board& board, int square);
        static bool isSquareUnderAttackBlack(const Board& board, int square);

        //appends moves to the nextBoards
        static void getWhitePawnsMoves(const Board& board, std::vector<Board>& nextBoards);
        static void getBlackPawnsMoves(const Board& board, std::vector<Board>& nextBoards);

        static void getWhiteKnightsMoves(const Board& board, std::vector<Board>& nextBoards);
        static void getBlackKnightsMoves(const Board& board, std::vector<Board>& nextBoards);

        static void getWhiteBishopsMoves(const Board& board, std::vector<Board>& nextBoards);
        static void getBlackBishopsMoves(const Board& board, std::vector<Board>& nextBoards);

        static void getWhiteRooksMoves(const Board& board, std::vector<Board>& nextBoards);
        static void getBlackRooksMoves(const Board& board, std::vector<Board>& nextBoards);

        static void getWhiteQueensMoves(const Board& board, std::vector<Board>& nextBoards);
        static void getBlackQueensMoves(const Board& board, std::vector<Board>& nextBoards);

        static void getWhiteKingMoves(const Board& board, std::vector<Board>& nextBoards);
        static void getBlackKingMoves(const Board& board, std::vector<Board>& nextBoards);

        static std::pair<int, int> getFromToPairWhite(const Board& nextBoard,
            const Chess::Board& board)
        {
            // For white moves
            uint64_t currentPieces;
            uint64_t nextPieces;

            currentPieces = board.getBitBoard().getAllWhitePieces();
            nextPieces = nextBoard.getBitBoard().getAllWhitePieces();

            // XOR will give us both the 'from' and 'to' squares
            uint64_t changedSquares = currentPieces ^ nextPieces;

            // The 'from' square will be set in currentWhitePieces but not in nextWhitePieces
            int fromSquare = std::countr_zero(changedSquares & currentPieces);

            // The 'to' square will be set in nextWhitePieces but not in currentWhitePieces
            int toSquare = std::countr_zero(changedSquares & nextPieces);
            return { fromSquare, toSquare };
        }

        static std::pair<int, int> getFromToPairBlack(const Board& nextBoard,
            const Chess::Board& board)
        {
            // For moves
            uint64_t currentPieces;
            uint64_t nextPieces;

            currentPieces = board.getBitBoard().getAllBlackPieces();
            nextPieces = nextBoard.getBitBoard().getAllBlackPieces();

            // XOR will give us both the 'from' and 'to' squares
            uint64_t changedSquares = currentPieces ^ nextPieces;

            // The 'from' square will be set in currentWhitePieces but not in nextWhitePieces
            int fromSquare = std::countr_zero(changedSquares & currentPieces);

            // The 'to' square will be set in nextWhitePieces but not in currentWhitePieces
            int toSquare = std::countr_zero(changedSquares & nextPieces);
            return { fromSquare, toSquare };
        }

        static std::pair<int, int> getFromToPair(uint64_t nextBitBoard, uint64_t bitBoard)
        {
            // XOR will give us both the 'from' and 'to' squares
            uint64_t changedSquares = bitBoard ^ nextBitBoard;

            // The 'from' square will be set in currentWhitePieces but not in nextWhitePieces
            int fromSquare = std::countr_zero(changedSquares & bitBoard);

            // The 'to' square will be set in nextWhitePieces but not in currentWhitePieces
            int toSquare = std::countr_zero(changedSquares & nextBitBoard);
            return { fromSquare, toSquare };
        }
    };


}