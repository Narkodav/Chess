#pragma once
#include "Utilities/ArrayNd.h"

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <bit>

#include "Constants.h"
#include "MagicBishops.h"
#include "MagicRooks.h"
#include "Flag.h"

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

    class Board
    {
    public:
        struct BitBoard {            
        private:
            static constexpr size_t pieceIndex(PieceTypes type) {
                return static_cast<size_t>(type) - 1;
            }

            // One 64-bit integer for each piece type
            std::array<uint64_t, static_cast<size_t>(PieceTypes::NUM) - 1> pieceMasks; // -1 because 0 is empty piece

        public:
            BitBoard() : pieceMasks({ 0 }) {};

            BitBoard(const BitBoard&) = default;
            BitBoard& operator=(const BitBoard&) = default;
            BitBoard(BitBoard&&) = default;
            BitBoard& operator=(BitBoard&&) = default;

            // Get all white pieces
            inline uint64_t getAllWhitePieces() const
            {
                return pieceMasks[pieceIndex(PieceTypes::WHITE_BISHOP)] |
                    pieceMasks[pieceIndex(PieceTypes::WHITE_KING)] |
                    pieceMasks[pieceIndex(PieceTypes::WHITE_KNIGHT)] |
                    pieceMasks[pieceIndex(PieceTypes::WHITE_PAWN)] |
                    pieceMasks[pieceIndex(PieceTypes::WHITE_QUEEN)] |
                    pieceMasks[pieceIndex(PieceTypes::WHITE_ROOK)];
            }

            inline uint64_t getAllBlackPieces() const
            {
                return pieceMasks[pieceIndex(PieceTypes::BLACK_BISHOP)] |
                    pieceMasks[pieceIndex(PieceTypes::BLACK_KING)] |
                    pieceMasks[pieceIndex(PieceTypes::BLACK_KNIGHT)] |
                    pieceMasks[pieceIndex(PieceTypes::BLACK_PAWN)] |
                    pieceMasks[pieceIndex(PieceTypes::BLACK_QUEEN)] |
                    pieceMasks[pieceIndex(PieceTypes::BLACK_ROOK)];
            }

            inline uint64_t& getPieceMask(PieceTypes type)
            {
                return pieceMasks[pieceIndex(type)];
            }

            inline const uint64_t& getPieceMask(PieceTypes type) const
            {
                return pieceMasks[pieceIndex(type)];
            }
            
            inline uint64_t getAllPieces() const
            {
                return getAllWhitePieces() | getAllBlackPieces();
            }

            static bool isOccupied(int rank, int file, uint64_t pieces)
            {
                return (pieces & (1ULL << rank * 8 + file)) != 0;
            }

        };

        struct Move {
            enum class Flags : uint8_t {
                QUIET = 0,
                KING_CASTLE = 1,
                QUEEN_CASTLE = 2,
                DOUBLE_PAWN_PUSH = 3,
                EN_PASSANT = 4,
                PROMOTION = 5, //can in theory fit 8 mutually exclusive flags
                CAPTURE = 1 << 3, //a separate flag for ai
                //last 4 bits are for for piece that the pawn was promoted to, default empty (12)
            };

            uint8_t fromSquare;     // 0-63
            uint8_t toSquare;       // 0-63
            uint8_t pieceTypes;     // first 4 bytes is moved piece and last 4 bytes is captured piece
            uint8_t flags;          // special move flags (castling, en passant, promotion)

            bool hasFlag(Flags flag) const {
                return (flags & static_cast<uint8_t>(flag)) == static_cast<uint8_t>(flag);
            }

            void setFlag(Flags flag)
            {
                flags |= static_cast<uint8_t>(flag); //doesn't check for mutually exclusive flags
            }

            void clearFlags()
            {
                flags = 0;
            }

            void clearMutuallyExclusiveFlag()
            {
                flags = (flags >> 3) << 3;
            }

            PieceTypes getPawnPromotion()
            {
                return static_cast<PieceTypes>(flags >> 4);
            }

            void setPawnPromotion(PieceTypes promotion)
            {
                flags |= static_cast<uint8_t>(promotion) << 4;
            }

            PieceTypes getCapturedPiece()
            {
                return static_cast<PieceTypes>(flags >> 4);
            }

            PieceTypes getMovedPiece()
            {
                return static_cast<PieceTypes>(flags & 0b00001111);
            }

            void setCapturedPiece(PieceTypes type)
            {
                flags |= static_cast<uint8_t>(type) << 4;
            }

            void setMovedPiece(PieceTypes type)
            {
                flags |= static_cast<uint8_t>(type);
            }

            Move() : fromSquare(0), toSquare(0), pieceTypes(0), flags(static_cast<uint8_t>(0)) {}
            Move(uint8_t from, uint8_t to, PieceTypes movedPiece, Flags moveFlag = Flags::QUIET,
                PieceTypes capturedPiece = PieceTypes::EMPTY, PieceTypes pawnPromotion = PieceTypes::EMPTY)
                : fromSquare(from), toSquare(to),
                pieceTypes(static_cast<uint8_t>(movedPiece) | (static_cast<uint8_t>(capturedPiece) << 4)),
                flags(static_cast<uint8_t>(moveFlag) | static_cast<uint8_t>(pawnPromotion) << 4) {}

            Move(const Move&) = default;
            Move& operator=(const Move&) = default;
            Move(Move&&) = default;
            Move& operator=(Move&&) = default;
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
        uint64_t m_enPassantMask = 0; //square on which an en passant capture is possible
        Flag<Flags> m_flags;
        Move m_lastMove;

    public:
        inline const BitBoard& getBitBoard() const { return m_bitBoard; };
        inline BitBoard& getBitBoard() { return m_bitBoard; };

        inline const uint64_t& getEnPassantMask() const { return m_enPassantMask; };
        inline uint64_t& getEnPassantMask() { return m_enPassantMask; };

        inline const Flag<Flags>& getFlags() const { return m_flags; };
        inline Flag<Flags>& getFlags() { return m_flags; };

        bool isWhiteChecked() const { return m_flags.has(Flags::WHITE_CHECKED); };
        bool isBlackChecked() const { return m_flags.has(Flags::BLACK_CHECKED); };

        Board() : m_bitBoard(), m_enPassantMask(0), m_flags() {};

        Board(const Board&) = default;
        Board& operator=(const Board&) = default;

        Board(Board&&) = default;
        Board& operator=(Board&&) = default;

        void reset()
        {
            // Reset white pieces
            m_bitBoard.getPieceMask(PieceTypes::WHITE_PAWN) = 0x000000000000FF00ULL;    // Rank 2
            m_bitBoard.getPieceMask(PieceTypes::WHITE_KNIGHT) = 0x0000000000000042ULL;    // b1, g1
            m_bitBoard.getPieceMask(PieceTypes::WHITE_BISHOP) = 0x0000000000000024ULL;    // c1, f1
            m_bitBoard.getPieceMask(PieceTypes::WHITE_ROOK) = 0x0000000000000081ULL;    // a1, h1
            m_bitBoard.getPieceMask(PieceTypes::WHITE_QUEEN) = 0x0000000000000008ULL;    // d1
            m_bitBoard.getPieceMask(PieceTypes::WHITE_KING) = 0x0000000000000010ULL;    // e1

            //Reset black pieces
            m_bitBoard.getPieceMask(PieceTypes::BLACK_PAWN) = 0x00FF000000000000ULL;    // Rank 7
            m_bitBoard.getPieceMask(PieceTypes::BLACK_KNIGHT) = 0x4200000000000000ULL;    // b8, g8
            m_bitBoard.getPieceMask(PieceTypes::BLACK_BISHOP) = 0x2400000000000000ULL;    // c8, f8
            m_bitBoard.getPieceMask(PieceTypes::BLACK_ROOK) = 0x8100000000000000ULL;    // a8, h8
            m_bitBoard.getPieceMask(PieceTypes::BLACK_QUEEN) = 0x0800000000000000ULL;    // d8
            m_bitBoard.getPieceMask(PieceTypes::BLACK_KING) = 0x1000000000000000ULL;    // e8

            m_flags.set(Flags::WHITE_HAS_CASTLING_KINGSIDE_RIGHTS,
                Flags::WHITE_HAS_CASTLING_QUEENSIDE_RIGHTS,
                Flags::BLACK_HAS_CASTLING_KINGSIDE_RIGHTS,
                Flags::BLACK_HAS_CASTLING_QUEENSIDE_RIGHTS);
                
            // Reset en passant square
            m_enPassantMask = 0;
        }

        PieceTypes getPieceAtSquare(int square) const {
            uint64_t squareMask = 1ULL << square;
            if (~m_bitBoard.getAllPieces() & squareMask)
                return PieceTypes::EMPTY;

            // White pieces
            if (m_bitBoard.getPieceMask(PieceTypes::WHITE_PAWN) & squareMask)
                return PieceTypes::WHITE_PAWN;
            if (m_bitBoard.getPieceMask(PieceTypes::WHITE_KNIGHT) & squareMask)
                return PieceTypes::WHITE_KNIGHT;
            if (m_bitBoard.getPieceMask(PieceTypes::WHITE_BISHOP) & squareMask)
                return PieceTypes::WHITE_BISHOP;
            if (m_bitBoard.getPieceMask(PieceTypes::WHITE_ROOK) & squareMask)
                return PieceTypes::WHITE_ROOK;
            if (m_bitBoard.getPieceMask(PieceTypes::WHITE_QUEEN) & squareMask)
                return PieceTypes::WHITE_QUEEN;
            if (m_bitBoard.getPieceMask(PieceTypes::WHITE_KING) & squareMask)
                return PieceTypes::WHITE_KING;

            // Black pieces
            if (m_bitBoard.getPieceMask(PieceTypes::BLACK_PAWN) & squareMask)
                return PieceTypes::BLACK_PAWN;
            if (m_bitBoard.getPieceMask(PieceTypes::BLACK_KNIGHT) & squareMask)
                return PieceTypes::BLACK_KNIGHT;
            if (m_bitBoard.getPieceMask(PieceTypes::BLACK_BISHOP) & squareMask)
                return PieceTypes::BLACK_BISHOP;
            if (m_bitBoard.getPieceMask(PieceTypes::BLACK_ROOK) & squareMask)
                return PieceTypes::BLACK_ROOK;
            if (m_bitBoard.getPieceMask(PieceTypes::BLACK_QUEEN) & squareMask)
                return PieceTypes::BLACK_QUEEN;
            if (m_bitBoard.getPieceMask(PieceTypes::BLACK_KING) & squareMask)
                return PieceTypes::BLACK_KING;

            return PieceTypes::EMPTY;
        }

        Move& getLastMove() { return m_lastMove; };
        const Move& getLastMove() const { return m_lastMove; };
    };

    class Calculator
    {
    public:

        static std::vector<Board> getNextBoardsWhite(const Board& currentBoard) {
            int i = 0;
            std::vector<Board> nextBoards;
            nextBoards.reserve(MAXIMUM_CONSERVATIVE_MOVE_AMOUNT);
            getWhitePawnMoves(currentBoard, nextBoards);
            getWhiteKnightMoves(currentBoard, nextBoards);
            getWhiteBishopMoves(currentBoard, nextBoards);
            getWhiteRookMoves(currentBoard, nextBoards);
            getWhiteQueenMoves(currentBoard, nextBoards);
            getWhiteKingMoves(currentBoard, nextBoards);
            return nextBoards;
        }

        static std::vector<Board> getNextBoardsBlack(const Board& currentBoard) {
            int i = 0;
            std::vector<Board> nextBoards;
            nextBoards.reserve(MAXIMUM_CONSERVATIVE_MOVE_AMOUNT);
            getBlackPawnMoves(currentBoard, nextBoards);
            getBlackKnightMoves(currentBoard, nextBoards);
            getBlackBishopMoves(currentBoard, nextBoards);
            getBlackRookMoves(currentBoard, nextBoards);
            getBlackQueenMoves(currentBoard, nextBoards);
            getBlackKingMoves(currentBoard, nextBoards);
            return nextBoards;
        }

        static std::unordered_multimap<int, Board> getNextBoardsWhiteMultimap(const Board& currentBoard) {

            std::vector<Board> nextBoards;
            nextBoards.reserve(MAXIMUM_CONSERVATIVE_MOVE_AMOUNT);
            getWhitePawnMoves(currentBoard, nextBoards);
            getWhiteKnightMoves(currentBoard, nextBoards);
            getWhiteBishopMoves(currentBoard, nextBoards);
            getWhiteRookMoves(currentBoard, nextBoards);
            getWhiteQueenMoves(currentBoard, nextBoards);
            getWhiteKingMoves(currentBoard, nextBoards);

            std::unordered_multimap<int, Board> nextMap;

            for (auto& nextBoard : nextBoards)
                nextMap.insert(std::make_pair(nextBoard.getLastMove().fromSquare, std::move(nextBoard)));

            return nextMap;
        }

        static  std::unordered_multimap<int, Board> getNextBoardsBlackMultimap(const Board& currentBoard) {

            std::vector<Board> nextBoards;
            nextBoards.reserve(MAXIMUM_CONSERVATIVE_MOVE_AMOUNT);
            getBlackPawnMoves(currentBoard, nextBoards);
            getBlackKnightMoves(currentBoard, nextBoards);
            getBlackBishopMoves(currentBoard, nextBoards);
            getBlackRookMoves(currentBoard, nextBoards);
            getBlackQueenMoves(currentBoard, nextBoards);
            getBlackKingMoves(currentBoard, nextBoards);

            std::unordered_multimap<int, Board> nextMap;

            for (auto& nextBoard : nextBoards)
                nextMap.insert(std::make_pair(nextBoard.getLastMove().fromSquare, std::move(nextBoard)));

            return nextMap;
        }

        //appends moves to the nextBoards
        static void getWhitePawnMoves(const Board& board, std::vector<Board>& nextBoards);
        static void getBlackPawnMoves(const Board& board, std::vector<Board>& nextBoards);

        static void getWhiteKnightMoves(const Board& board, std::vector<Board>& nextBoards);
        static void getBlackKnightMoves(const Board& board, std::vector<Board>& nextBoards);

        static void getWhiteBishopMoves(const Board& board, std::vector<Board>& nextBoards);
        static void getBlackBishopMoves(const Board& board, std::vector<Board>& nextBoards);

        static void getWhiteRookMoves(const Board& board, std::vector<Board>& nextBoards);
        static void getBlackRookMoves(const Board& board, std::vector<Board>& nextBoards);

        static void getWhiteQueenMoves(const Board& board, std::vector<Board>& nextBoards);
        static void getBlackQueenMoves(const Board& board, std::vector<Board>& nextBoards);

        static void getWhiteKingMoves(const Board& board, std::vector<Board>& nextBoards);
        static void getBlackKingMoves(const Board& board, std::vector<Board>& nextBoards);

        //pawn attack moves
        static inline uint64_t pawnAttackForwardLeft(uint64_t squareMask)
        {
            return (squareMask & ~FILE_A) << 7;
        }

        static inline uint64_t pawnAttackForwardRight(uint64_t squareMask)
        {
            return (squareMask & ~FILE_H) << 9;
        }

        static inline uint64_t pawnAttackBackwardLeft(uint64_t squareMask)
        {
            return (squareMask & ~FILE_A) >> 9;
        }

        static inline uint64_t pawnAttackBackwardRight(uint64_t squareMask)
        {
            return (squareMask & ~FILE_H) >> 7;
        }

        //versions with an enemy check
        static inline uint64_t pawnAttackForwardLeft(uint64_t squareMask, uint64_t enemyMask)
        {
            return ((squareMask & ~FILE_A) << 7) & enemyMask;
        }

        static inline uint64_t pawnAttackForwardRight(uint64_t squareMask, uint64_t enemyMask)
        {
            return ((squareMask & ~FILE_H) << 9) & enemyMask;
        }

        static inline uint64_t pawnAttackBackwardLeft(uint64_t squareMask, uint64_t enemyMask)
        {
            return ((squareMask & ~FILE_A) >> 9) & enemyMask;
        }

        static inline uint64_t pawnAttackBackwardRight(uint64_t squareMask, uint64_t enemyMask)
        {
            return ((squareMask & ~FILE_H) >> 7) & enemyMask;
        }

        //generic moves without checks
        static inline uint64_t moveForward(uint64_t pawnMask)
        {
            return pawnMask << 8;
        }

        static inline uint64_t moveBackward(uint64_t pawnMask)
        {
            return pawnMask >> 8;
        }

        static inline uint64_t moveLeftForward(uint64_t pawnMask)
        {
            return pawnMask << 7;
        }

        static inline uint64_t moveRightForward(uint64_t pawnMask)
        {
            return pawnMask << 9;
        }

        static inline uint64_t moveLeftBackward(uint64_t pawnMask)
        {
            return pawnMask >> 9;
        }

        static inline uint64_t moveRightBackward(uint64_t pawnMask)
        {
            return pawnMask >> 7;
        }

        //versions with an enemy check
        static inline uint64_t pawnMoveForward(uint64_t pawnMask, uint64_t empty)
        {
            return (pawnMask << 8) & empty;
        }

        static inline uint64_t pawnMoveBackward(uint64_t pawnMask, uint64_t empty)
        {
            return (pawnMask >> 8) & empty;
        }

        static inline uint64_t pawnMoveForwardDouble(uint64_t pawnMask, uint64_t empty)
        {
            return (((pawnMask & RANK_2) << 8) & empty) << 8 & empty;
        }

        static inline uint64_t pawnMoveBackwardDouble(uint64_t pawnMask, uint64_t empty)
        {
            return (((pawnMask & RANK_7) >> 8) & empty) >> 8 & empty;
        }

        //should promote checks
        static inline uint64_t pawnShouldPromoteWhite(uint64_t pawnMask)
        {
            return pawnMask & RANK_8;
        }

        static inline uint64_t pawnShouldPromoteBlack(uint64_t pawnMask)
        {
            return pawnMask & RANK_1;
        }

        //calculates a mask to detect black pawn attacks
        static inline uint64_t pawnBlackCalculator(uint64_t squareMask)
        {
            return pawnAttackForwardLeft(squareMask) | pawnAttackForwardRight(squareMask);
        }

        static inline uint64_t pawnWhiteCalculator(uint64_t squareMask)
        {
            return pawnAttackBackwardLeft(squareMask) | pawnAttackBackwardRight(squareMask);
        }

        //use premade pawn attack calculators
        template<typename PawnCalculator>
        static inline bool isSquareUnderAttack(
            const Board& board, uint64_t squareMask, int square,
            uint64_t enemyPawns, uint64_t enemyKnights,
            uint64_t enemyBishops, uint64_t enemyRooks,
            uint64_t enemyQueens, uint64_t enemyKing,
            PawnCalculator&& pawnAttacksCalculator)
        {
            uint64_t occupancy = board.getBitBoard().getAllPieces();
            // Pawn attacks
            uint64_t pawnAttacks = pawnAttacksCalculator(squareMask);

            if (pawnAttacks & enemyPawns)
                return true;

            // Knight attacks
            if (KNIGHT_ATTACKS[square] & enemyKnights)
                return true;

            // King attacks (needed for king move validation)
            if (KING_ATTACKS[square] & enemyKing)
                return true;

            // Bishop/Queen attacks
            if (MagicBishops::getAttacks(square, occupancy) &
                (enemyBishops | enemyQueens))
                return true;

            // Rook/Queen attacks
            if (MagicRooks::getAttacks(square, occupancy) &
                (enemyRooks | enemyQueens))
                return true;
            return false;
        }

        struct PawnMoveVariablePack {
            PieceTypes friendlyPawnType;
            PieceTypes friendlyKnightType;
            PieceTypes friendlyBishopType;
            PieceTypes friendlyRookType;
            PieceTypes friendlyQueenType;
            PieceTypes friendlyKingType;

            PieceTypes enemyPawnType;
            PieceTypes enemyKnightType;
            PieceTypes enemyBishopType;
            PieceTypes enemyRookType;
            PieceTypes enemyQueenType;
            PieceTypes enemyKingType;

            Board::Flags friendlyCheckFlag; //if moving white this is white
            Board::Flags enemyCheckFlag;    //if moving black this is black
        };

        template<typename EnemyCalculator, typename PackType>
        static inline bool enemyChecked(const Board& board, const PackType& pack,
            EnemyCalculator&& enemyPawnAttacksCalculator)
        {
            if (isSquareUnderAttack(board, board.getBitBoard().getPieceMask(pack.enemyKingType),
                std::countr_zero(board.getBitBoard().getPieceMask(pack.enemyKingType)),
                board.getBitBoard().getPieceMask(pack.friendlyPawnType),
                board.getBitBoard().getPieceMask(pack.friendlyKnightType),
                board.getBitBoard().getPieceMask(pack.friendlyBishopType),
                board.getBitBoard().getPieceMask(pack.friendlyRookType),
                board.getBitBoard().getPieceMask(pack.friendlyQueenType),
                board.getBitBoard().getPieceMask(pack.friendlyKingType),
                enemyPawnAttacksCalculator))
                return true;
            return false;
        }

        template<typename FriendlyCalculator, typename PackType>
        static inline bool friendlyChecked(const Board& board, const PackType& pack,
            FriendlyCalculator&& friendlyPawnAttacksCalculator)
        {
            if (isSquareUnderAttack(board, board.getBitBoard().getPieceMask(pack.friendlyKingType),
                std::countr_zero(board.getBitBoard().getPieceMask(pack.friendlyKingType)),
                board.getBitBoard().getPieceMask(pack.enemyPawnType),
                board.getBitBoard().getPieceMask(pack.enemyKnightType),
                board.getBitBoard().getPieceMask(pack.enemyBishopType),
                board.getBitBoard().getPieceMask(pack.enemyRookType),
                board.getBitBoard().getPieceMask(pack.enemyQueenType),
                board.getBitBoard().getPieceMask(pack.enemyKingType),
                friendlyPawnAttacksCalculator))
                return true;
            return false;
        }

        template<typename MoveDiagonalCapture, typename MoveBackDiagonal,
            typename ShouldPromote, typename FriendlyCalculator,
            typename EnemyCalculator>
        static inline void pawnCapture(const Board& board,
            std::vector<Board>& nextBoards,
            const PawnMoveVariablePack& pack, uint64_t enemies,
            MoveDiagonalCapture&& moveDiagonalCapture,
            MoveBackDiagonal&& moveBackDiagonal,
            ShouldPromote&& shouldPromote,
            //used to determine if friendly king is under attack
            FriendlyCalculator&& friendlyPawnAttacksCalculator,
            //used to determine if enemy king is under attack
            EnemyCalculator&& enemyPawnAttacksCalculator)
        {
            uint64_t moves = moveDiagonalCapture(board.getBitBoard().getPieceMask(pack.friendlyPawnType), enemies);
            while (moves) {
                int destinationSquare = std::countr_zero(moves);
                uint64_t destinationMask = 1ULL << destinationSquare;

                Board newBoard = board;

                newBoard.getEnPassantMask() = 0; //set en passant to 0 for next move
                newBoard.getFlags().clear(pack.friendlyCheckFlag);

                // Find the source square (the pawn that can move to this destination)
                uint64_t sourceMask = moveBackDiagonal(destinationMask);
                uint64_t sourceSquare = std::countr_zero(sourceMask);

                Board::Move move(sourceSquare, destinationSquare, pack.friendlyPawnType,
                    Board::Move::Flags::CAPTURE);

                //goto used for performance reasons
                if ((newBoard.getBitBoard().getPieceMask(pack.enemyPawnType) &= ~destinationMask) !=
                    board.getBitBoard().getPieceMask(pack.enemyPawnType))
                {
                    move.setCapturedPiece(pack.enemyPawnType);
                    goto CAPTURE_FOUND;
                }
                if ((newBoard.getBitBoard().getPieceMask(pack.enemyKnightType) &= ~destinationMask) !=
                    board.getBitBoard().getPieceMask(pack.enemyKnightType))
                {
                    move.setCapturedPiece(pack.enemyKnightType);
                    goto CAPTURE_FOUND;
                }
                if ((newBoard.getBitBoard().getPieceMask(pack.enemyBishopType) &= ~destinationMask) !=
                    board.getBitBoard().getPieceMask(pack.enemyBishopType))
                {
                    move.setCapturedPiece(pack.enemyBishopType);
                    goto CAPTURE_FOUND;
                }
                if ((newBoard.getBitBoard().getPieceMask(pack.enemyRookType) &= ~destinationMask) !=
                    board.getBitBoard().getPieceMask(pack.enemyRookType))
                {
                    move.setCapturedPiece(pack.enemyRookType);
                    goto CAPTURE_FOUND;
                }
                newBoard.getBitBoard().getPieceMask(pack.enemyQueenType) &= ~destinationMask;
                move.setCapturedPiece(pack.enemyQueenType);

            CAPTURE_FOUND:

                newBoard.getLastMove() = move;
                newBoard.getBitBoard().getPieceMask(pack.friendlyPawnType) &= ~sourceMask;
                if (shouldPromote(destinationMask))
                {
                    //will add support for other pieces later, requires reworking the ui
                    newBoard.getBitBoard().getPieceMask(pack.friendlyQueenType) |= destinationMask;
                    move.setFlag(Board::Move::Flags::PROMOTION);
                    move.setPawnPromotion(pack.friendlyQueenType);
                }
                else newBoard.getBitBoard().getPieceMask(pack.friendlyPawnType) |= destinationMask;

                //check if enemy under attack
                if (enemyChecked(newBoard, pack, enemyPawnAttacksCalculator))
                    newBoard.getFlags().set(pack.enemyCheckFlag);
                //check if friendly under attack
                if (!friendlyChecked(newBoard, pack, friendlyPawnAttacksCalculator))
                    nextBoards.push_back(newBoard);

                moves &= moves - 1;
            }
        }

        template<typename MoveForward, typename MoveForwardDouble, typename MoveBackward,
            typename MoveLeftCapture, typename MoveRightCapture,
            typename MoveRightBack, typename MoveLeftBack,
            typename ShouldPromote, typename FriendlyCalculator, typename EnemyCalculator>
        static void getPawnMoves(const Board& board, std::vector<Board>& nextBoards,
            const PawnMoveVariablePack& pack, uint64_t enPassantRank,

            MoveForward&& moveForward,
            MoveForwardDouble&& moveForwardDouble,
            MoveBackward&& moveBackward,

            MoveLeftCapture&& moveLeftCapture,
            MoveRightCapture&& moveRightCapture,

            MoveRightBack&& moveRightBack,
            MoveLeftBack&& moveLeftBack,

            ShouldPromote&& shouldPromote,

            //used to determine if friendly king is under attack
            FriendlyCalculator&& friendlyPawnAttacksCalculator,
            //used to determine if enemy king is under attack
            EnemyCalculator&& enemyPawnAttacksCalculator)
        {
            uint64_t empty = ~board.getBitBoard().getAllPieces();
            uint64_t enemies =
                board.getBitBoard().getPieceMask(pack.enemyPawnType) |
                board.getBitBoard().getPieceMask(pack.enemyKnightType) |
                board.getBitBoard().getPieceMask(pack.enemyBishopType) |
                board.getBitBoard().getPieceMask(pack.enemyRookType) |
                board.getBitBoard().getPieceMask(pack.enemyQueenType) |
                board.getBitBoard().getPieceMask(pack.enemyKingType);

            uint64_t moves = 0;

            // Single push
            moves = moveForward(board.getBitBoard().getPieceMask(pack.friendlyPawnType), empty);
            while (moves) {
                int destinationSquare = std::countr_zero(moves);
                uint64_t destinationMask = 1ULL << destinationSquare;

                Board newBoard = board;
                newBoard.getEnPassantMask() = 0; //set en passant to 0 for next move
                newBoard.getFlags().clear(pack.friendlyCheckFlag);

                // Find the source square (the pawn that can move to this destination)
                uint64_t sourceMask = moveBackward(destinationMask);
                uint64_t sourceSquare = std::countr_zero(sourceMask);

                Board::Move move(sourceSquare, destinationSquare, pack.friendlyPawnType);

                newBoard.getBitBoard().getPieceMask(pack.friendlyPawnType) &= ~sourceMask;
                if (shouldPromote(destinationMask))
                {
                    //will add support for other pieces later, requires reworking the ui
                    newBoard.getBitBoard().getPieceMask(pack.friendlyQueenType) |= destinationMask;
                    move.setFlag(Board::Move::Flags::PROMOTION);
                    move.setPawnPromotion(pack.friendlyQueenType);
                }
                else newBoard.getBitBoard().getPieceMask(pack.friendlyPawnType) |= destinationMask;
                newBoard.getLastMove() = move;

                //check if enemy under attack
                if (enemyChecked(newBoard, pack, enemyPawnAttacksCalculator))
                    newBoard.getFlags().set(pack.enemyCheckFlag);
                //check if friendly under attack
                if (!friendlyChecked(newBoard, pack, friendlyPawnAttacksCalculator))
                    nextBoards.push_back(newBoard);

                moves &= moves - 1;
            }

            // Double push
            // lambda already accounts for rank checking
            moves = moveForwardDouble(board.getBitBoard().getPieceMask(pack.friendlyPawnType), empty);
            while (moves) {
                int destinationSquare = std::countr_zero(moves);
                uint64_t destinationMask = 1ULL << destinationSquare;

                Board newBoard = board;

                newBoard.getFlags().clear(pack.friendlyCheckFlag);

                // Find the source square (the pawn that can move to this destination)
                uint64_t enPassantMask = moveBackward(destinationMask);
                newBoard.getEnPassantMask() = enPassantMask; //set en passant to the mask for next move

                uint64_t sourceMask = moveBackward(enPassantMask);
                uint64_t sourceSquare = std::countr_zero(sourceMask);

                Board::Move move(sourceSquare, destinationSquare, pack.friendlyPawnType,
                    Board::Move::Flags::DOUBLE_PAWN_PUSH);
                newBoard.getLastMove() = move;

                newBoard.getBitBoard().getPieceMask(pack.friendlyPawnType) &= ~sourceMask;
                newBoard.getBitBoard().getPieceMask(pack.friendlyPawnType) |= destinationMask;

                //check if enemy under attack
                if (enemyChecked(newBoard, pack, enemyPawnAttacksCalculator))
                    newBoard.getFlags().set(pack.enemyCheckFlag);
                //check if friendly under attack
                if (!friendlyChecked(newBoard, pack, friendlyPawnAttacksCalculator))
                    nextBoards.push_back(newBoard);

                moves &= moves - 1;
            }

            // Captures
            // Left captures
            pawnCapture(board, nextBoards, pack, enemies, moveLeftCapture, moveRightBack, shouldPromote,
                friendlyPawnAttacksCalculator, enemyPawnAttacksCalculator);


            // Right captures (shift up and right, but not if on h-file)
            pawnCapture(board, nextBoards, pack, enemies, moveRightCapture, moveLeftBack, shouldPromote,
                friendlyPawnAttacksCalculator, enemyPawnAttacksCalculator);

            // En passant
            if ((board.getEnPassantMask() & enPassantRank) != 0)
            {
                // no while loops because multiple en passant opportunities are impossible 
                // and if a mask exists one move is guaranteed to exist
                moves = moveLeftCapture(board.getBitBoard().getPieceMask(pack.friendlyPawnType), board.getEnPassantMask());
                if (!moves)
                    moves = moveRightCapture(board.getBitBoard().getPieceMask(pack.friendlyPawnType), board.getEnPassantMask());
                if (!moves)
                    return;
                int destinationSquare = std::countr_zero(moves);
                uint64_t sourceMask = moveRightBack(moves);

                Board newBoard = board;
                newBoard.getEnPassantMask() = 0; //set en passant to 0 for next move
                newBoard.getFlags().clear(pack.friendlyCheckFlag);

                uint64_t sourceSquare = std::countr_zero(sourceMask);

                Board::Move move(sourceSquare, destinationSquare, pack.friendlyPawnType,
                    Board::Move::Flags::EN_PASSANT, pack.enemyPawnType);
                move.setFlag(Board::Move::Flags::CAPTURE);
                newBoard.getLastMove() = move;

                newBoard.getBitBoard().getPieceMask(pack.enemyPawnType) &= ~(moveBackward(board.getEnPassantMask()));
                newBoard.getBitBoard().getPieceMask(pack.friendlyPawnType) &= ~sourceMask;
                newBoard.getBitBoard().getPieceMask(pack.friendlyPawnType) |= moves;

                //check if enemy under attack
                if (enemyChecked(newBoard, pack, enemyPawnAttacksCalculator))
                    newBoard.getFlags().set(pack.enemyCheckFlag);
                //check if friendly under attack
                if (!friendlyChecked(newBoard, pack, friendlyPawnAttacksCalculator))
                    nextBoards.push_back(newBoard);
            }
        };

        struct LookupTableMovePack
        {
            PieceTypes pieceType;

            PieceTypes friendlyPawnType;
            PieceTypes friendlyKnightType;
            PieceTypes friendlyBishopType;
            PieceTypes friendlyRookType;
            PieceTypes friendlyQueenType;
            PieceTypes friendlyKingType;

            PieceTypes enemyPawnType;
            PieceTypes enemyKnightType;
            PieceTypes enemyBishopType;
            PieceTypes enemyRookType;
            PieceTypes enemyQueenType;
            PieceTypes enemyKingType;

            Board::Flags friendlyCheckFlag; //if moving white this is white
            Board::Flags enemyCheckFlag;    //if moving black this is black

            uint64_t friendlyPieces;
        };

        template<typename FriendlyCalculator, typename EnemyCalculator>
        static void getKnightMoves(const Board& board, std::vector<Board>& nextBoards,
            const LookupTableMovePack& pack,

            //used to determine if friendly king is under attack
            FriendlyCalculator&& friendlyPawnAttacksCalculator,
            //used to determine if enemy king is under attack
            EnemyCalculator&& enemyPawnAttacksCalculator)
        {
            uint64_t knights = board.getBitBoard().getPieceMask(pack.pieceType);

            while (knights) {
                int sourceSquare = std::countr_zero(knights);
                uint64_t sourceMask = 1ULL << sourceSquare;

                uint64_t destinationsSquaresMask = KNIGHT_ATTACKS[sourceSquare] & ~pack.friendlyPieces;
                while (destinationsSquaresMask) {
                    // Create working copy of current board
                    int destinationSquare = std::countr_zero(destinationsSquaresMask);
                    uint64_t destinationMask = 1ULL << destinationSquare;

                    Board newBoard = board;
                    newBoard.getEnPassantMask() = 0; //set en passant to 0 for next move
                    newBoard.getFlags().clear(pack.friendlyCheckFlag);
                    newBoard.getBitBoard().getPieceMask(pack.pieceType) &= ~sourceMask;
                    newBoard.getBitBoard().getPieceMask(pack.pieceType) |= destinationMask;

                    Board::Move move(sourceSquare, destinationSquare, pack.pieceType);

                    //goto used for performance reasons
                    if ((newBoard.getBitBoard().getPieceMask(pack.enemyPawnType) &= ~destinationMask) !=
                        board.getBitBoard().getPieceMask(pack.enemyPawnType))
                    {
                        move.setCapturedPiece(pack.enemyPawnType);
                        move.setFlag(Board::Move::Flags::CAPTURE);
                        goto CAPTURE_FOUND;
                    }
                    if ((newBoard.getBitBoard().getPieceMask(pack.enemyKnightType) &= ~destinationMask) !=
                        board.getBitBoard().getPieceMask(pack.enemyKnightType))
                    {
                        move.setCapturedPiece(pack.enemyKnightType);
                        move.setFlag(Board::Move::Flags::CAPTURE);
                        goto CAPTURE_FOUND;
                    }
                    if ((newBoard.getBitBoard().getPieceMask(pack.enemyBishopType) &= ~destinationMask) !=
                        board.getBitBoard().getPieceMask(pack.enemyBishopType))
                    {
                        move.setCapturedPiece(pack.enemyBishopType);
                        move.setFlag(Board::Move::Flags::CAPTURE);
                        goto CAPTURE_FOUND;
                    }
                    if ((newBoard.getBitBoard().getPieceMask(pack.enemyRookType) &= ~destinationMask) !=
                        board.getBitBoard().getPieceMask(pack.enemyRookType))
                    {
                        move.setCapturedPiece(pack.enemyRookType);
                        move.setFlag(Board::Move::Flags::CAPTURE);
                        goto CAPTURE_FOUND;
                    }
                    if ((newBoard.getBitBoard().getPieceMask(pack.enemyQueenType) &= ~destinationMask) !=
                        board.getBitBoard().getPieceMask(pack.enemyQueenType))
                    {
                        move.setCapturedPiece(pack.enemyQueenType);
                        move.setFlag(Board::Move::Flags::CAPTURE);
                        goto CAPTURE_FOUND;
                    }

                CAPTURE_FOUND:
                    newBoard.getLastMove() = move;
                    //check if enemy under attack
                    if (enemyChecked(newBoard, pack, enemyPawnAttacksCalculator))
                        newBoard.getFlags().set(pack.enemyCheckFlag);
                    //check if friendly under attack
                    if (!friendlyChecked(newBoard, pack, friendlyPawnAttacksCalculator))
                        nextBoards.push_back(newBoard);

                    destinationsSquaresMask &= destinationsSquaresMask - 1;
                }
                knights &= knights - 1;
            }
        };

        static inline uint64_t bishopLookupFunction(int square, size_t occupancy)
        {
            return MagicBishops::getAttacks(square, occupancy);
        }

        static inline uint64_t rookLookupFunction(int square, size_t occupancy)
        {
            return MagicRooks::getAttacks(square, occupancy);
        }

        static inline uint64_t queenLookupFunction(int square, size_t occupancy)
        {
            return MagicBishops::getAttacks(square, occupancy) | MagicRooks::getAttacks(square, occupancy);
        }

        template<typename FriendlyCalculator, typename EnemyCalculator, typename LookupFunction>
        static void getRookMoves(const Board& board, std::vector<Board>& nextBoards,
            const LookupTableMovePack& pack,
            uint64_t kingSideRookMask, uint64_t queenSideRookMask,
            Board::Flags kingSideCastlingRightsFlag, Board::Flags queenSideCastlingRightsFlag,
            LookupFunction&& lookupFunction,
            //used to determine if friendly king is under attack
            FriendlyCalculator&& friendlyPawnAttacksCalculator,
            //used to determine if enemy king is under attack
            EnemyCalculator&& enemyPawnAttacksCalculator)
        {
            uint64_t pieces = board.getBitBoard().getPieceMask(pack.pieceType);

            while (pieces) {
                int sourceSquare = std::countr_zero(pieces);
                uint64_t sourceMask = 1ULL << sourceSquare;

                uint64_t destinationsSquaresMask = lookupFunction(sourceSquare,
                    board.getBitBoard().getAllPieces()) & ~pack.friendlyPieces;
                while (destinationsSquaresMask) {
                    // Create working copy of current board
                    int destinationSquare = std::countr_zero(destinationsSquaresMask);
                    uint64_t destinationMask = 1ULL << destinationSquare;

                    Board newBoard = board;
                    newBoard.getEnPassantMask() = 0; //set en passant to 0 for next move
                    newBoard.getFlags().clear(pack.friendlyCheckFlag);
                    newBoard.getBitBoard().getPieceMask(pack.pieceType) &= ~sourceMask;
                    newBoard.getBitBoard().getPieceMask(pack.pieceType) |= destinationMask;

                    Board::Move move(sourceSquare, destinationSquare, pack.pieceType);
                    if (sourceMask & kingSideRookMask)
                        newBoard.getFlags().clear(kingSideCastlingRightsFlag);
                    else if (sourceMask & queenSideRookMask)
                        newBoard.getFlags().clear(queenSideCastlingRightsFlag);

                    //goto used for performance reasons
                    if ((newBoard.getBitBoard().getPieceMask(pack.enemyPawnType) &= ~destinationMask) !=
                        board.getBitBoard().getPieceMask(pack.enemyPawnType))
                    {
                        move.setCapturedPiece(pack.enemyPawnType);
                        move.setFlag(Board::Move::Flags::CAPTURE);
                        goto CAPTURE_FOUND;
                    }
                    if ((newBoard.getBitBoard().getPieceMask(pack.enemyKnightType) &= ~destinationMask) !=
                        board.getBitBoard().getPieceMask(pack.enemyKnightType))
                    {
                        move.setCapturedPiece(pack.enemyKnightType);
                        move.setFlag(Board::Move::Flags::CAPTURE);
                        goto CAPTURE_FOUND;
                    }
                    if ((newBoard.getBitBoard().getPieceMask(pack.enemyBishopType) &= ~destinationMask) !=
                        board.getBitBoard().getPieceMask(pack.enemyBishopType))
                    {
                        move.setCapturedPiece(pack.enemyBishopType);
                        move.setFlag(Board::Move::Flags::CAPTURE);
                        goto CAPTURE_FOUND;
                    }
                    if ((newBoard.getBitBoard().getPieceMask(pack.enemyRookType) &= ~destinationMask) !=
                        board.getBitBoard().getPieceMask(pack.enemyRookType))
                    {
                        move.setCapturedPiece(pack.enemyRookType);
                        move.setFlag(Board::Move::Flags::CAPTURE);
                        goto CAPTURE_FOUND;
                    }
                    if ((newBoard.getBitBoard().getPieceMask(pack.enemyQueenType) &= ~destinationMask) !=
                        board.getBitBoard().getPieceMask(pack.enemyQueenType))
                    {
                        move.setCapturedPiece(pack.enemyQueenType);
                        move.setFlag(Board::Move::Flags::CAPTURE);
                        goto CAPTURE_FOUND;
                    }

                CAPTURE_FOUND:
                    newBoard.getLastMove() = move;

                    //check if enemy under attack
                    if (enemyChecked(newBoard, pack, enemyPawnAttacksCalculator))
                        newBoard.getFlags().set(pack.enemyCheckFlag);
                    //check if friendly under attack
                    if (!friendlyChecked(newBoard, pack, friendlyPawnAttacksCalculator))
                        nextBoards.push_back(newBoard);

                    destinationsSquaresMask &= destinationsSquaresMask - 1;
                }
                pieces &= pieces - 1;
            }
        };

        //used for bishops and queens
        template<typename FriendlyCalculator, typename EnemyCalculator, typename LookupFunction>
        static void getSlidingMoves(const Board& board, std::vector<Board>& nextBoards,
            const LookupTableMovePack& pack,
            LookupFunction&& lookupFunction,
            //used to determine if friendly king is under attack
            FriendlyCalculator&& friendlyPawnAttacksCalculator,
            //used to determine if enemy king is under attack
            EnemyCalculator&& enemyPawnAttacksCalculator)
        {
            uint64_t pieces = board.getBitBoard().getPieceMask(pack.pieceType);

            while (pieces) {
                int sourceSquare = std::countr_zero(pieces);
                uint64_t sourceMask = 1ULL << sourceSquare;

                uint64_t destinationsSquaresMask = lookupFunction(sourceSquare,
                    board.getBitBoard().getAllPieces()) & ~pack.friendlyPieces;
                while (destinationsSquaresMask) {
                    // Create working copy of current board
                    int destinationSquare = std::countr_zero(destinationsSquaresMask);
                    uint64_t destinationMask = 1ULL << destinationSquare;

                    Board newBoard = board;
                    newBoard.getEnPassantMask() = 0; //set en passant to 0 for next move
                    newBoard.getFlags().clear(pack.friendlyCheckFlag);
                    newBoard.getBitBoard().getPieceMask(pack.pieceType) &= ~sourceMask;
                    newBoard.getBitBoard().getPieceMask(pack.pieceType) |= destinationMask;

                    Board::Move move(sourceSquare, destinationSquare, pack.pieceType);

                    //goto used for performance reasons
                    if ((newBoard.getBitBoard().getPieceMask(pack.enemyPawnType) &= ~destinationMask) !=
                        board.getBitBoard().getPieceMask(pack.enemyPawnType))
                    {
                        move.setCapturedPiece(pack.enemyPawnType);
                        move.setFlag(Board::Move::Flags::CAPTURE);
                        goto CAPTURE_FOUND;
                    }
                    if ((newBoard.getBitBoard().getPieceMask(pack.enemyKnightType) &= ~destinationMask) !=
                        board.getBitBoard().getPieceMask(pack.enemyKnightType))
                    {
                        move.setCapturedPiece(pack.enemyKnightType);
                        move.setFlag(Board::Move::Flags::CAPTURE);
                        goto CAPTURE_FOUND;
                    }
                    if ((newBoard.getBitBoard().getPieceMask(pack.enemyBishopType) &= ~destinationMask) !=
                        board.getBitBoard().getPieceMask(pack.enemyBishopType))
                    {
                        move.setCapturedPiece(pack.enemyBishopType);
                        move.setFlag(Board::Move::Flags::CAPTURE);
                        goto CAPTURE_FOUND;
                    }
                    if ((newBoard.getBitBoard().getPieceMask(pack.enemyRookType) &= ~destinationMask) !=
                        board.getBitBoard().getPieceMask(pack.enemyRookType))
                    {
                        move.setCapturedPiece(pack.enemyRookType);
                        move.setFlag(Board::Move::Flags::CAPTURE);
                        goto CAPTURE_FOUND;
                    }
                    if ((newBoard.getBitBoard().getPieceMask(pack.enemyQueenType) &= ~destinationMask) !=
                        board.getBitBoard().getPieceMask(pack.enemyQueenType))
                    {
                        move.setCapturedPiece(pack.enemyQueenType);
                        move.setFlag(Board::Move::Flags::CAPTURE);
                        goto CAPTURE_FOUND;
                    }

                CAPTURE_FOUND:
                    newBoard.getLastMove() = move;

                    //check if enemy under attack
                    if (enemyChecked(newBoard, pack, enemyPawnAttacksCalculator))
                        newBoard.getFlags().set(pack.enemyCheckFlag);
                    //check if friendly under attack
                    if (!friendlyChecked(newBoard, pack, friendlyPawnAttacksCalculator))
                        nextBoards.push_back(newBoard);

                    destinationsSquaresMask &= destinationsSquaresMask - 1;
                }
                pieces &= pieces - 1;
            }
        };

        struct KingMovePack
        {
            PieceTypes friendlyPawnType;
            PieceTypes friendlyKnightType;
            PieceTypes friendlyBishopType;
            PieceTypes friendlyRookType;
            PieceTypes friendlyQueenType;
            PieceTypes friendlyKingType;

            PieceTypes enemyPawnType;
            PieceTypes enemyKnightType;
            PieceTypes enemyBishopType;
            PieceTypes enemyRookType;
            PieceTypes enemyQueenType;
            PieceTypes enemyKingType;

            Board::Flags friendlyCheckFlag; //if moving white this is white
            Board::Flags enemyCheckFlag;    //if moving black this is black

            Board::Flags castlingRightsQueenSideFlag;
            Board::Flags castlingRightsKingSideFlag;

            uint64_t kingSideCastlingPathMask;
            uint64_t queenSideCastlingPathMask;

            uint64_t kingSideCastlingKingEndMask;
            uint64_t queenSideCastlingKingEndMask;

            uint64_t kingSideCastlingRookStartMask;
            uint64_t queenSideCastlingRookStartMask;
            uint64_t kingSideCastlingRookEndMask;
            uint64_t queenSideCastlingRookEndMask;

            uint64_t friendlyPieces;
        };

        static void getKingMoves(const Board& board, std::vector<Board>& nextBoards,
            const KingMovePack& pack,
            //used to determine if friendly king is under attack
            std::function<uint64_t(uint64_t)>&& friendlyPawnAttacksCalculator,
            //used to determine if enemy king is under attack
            std::function<uint64_t(uint64_t)>&& enemyPawnAttacksCalculator)
        {
            uint64_t empty = ~board.getBitBoard().getAllPieces();

            int sourceSquare = std::countr_zero(board.getBitBoard().getPieceMask(pack.friendlyKingType));
            uint64_t sourceMask = 1ULL << sourceSquare;

            uint64_t destinationsMask = KING_ATTACKS[sourceSquare] & ~pack.friendlyPieces;

            while (destinationsMask) {
                // Create working copy of current board
                int destinationSquare = std::countr_zero(destinationsMask);
                uint64_t destinationMask = 1ULL << destinationSquare;

                Board newBoard = board;
                newBoard.getFlags().clear(pack.friendlyCheckFlag);
                newBoard.getFlags().clear(pack.castlingRightsQueenSideFlag);
                newBoard.getFlags().clear(pack.castlingRightsKingSideFlag);
                newBoard.getEnPassantMask() = 0; //set en passant to 0 for next move

                newBoard.getBitBoard().getPieceMask(pack.friendlyKingType) &= ~sourceMask;
                newBoard.getBitBoard().getPieceMask(pack.friendlyKingType) |= destinationMask;

                Board::Move move(sourceSquare, destinationSquare, pack.friendlyKingType);

                //goto used for performance reasons
                if ((newBoard.getBitBoard().getPieceMask(pack.enemyPawnType) &= ~destinationMask) !=
                    board.getBitBoard().getPieceMask(pack.enemyPawnType))
                {
                    move.setCapturedPiece(pack.enemyPawnType);
                    move.setFlag(Board::Move::Flags::CAPTURE);
                    goto CAPTURE_FOUND;
                }
                if ((newBoard.getBitBoard().getPieceMask(pack.enemyKnightType) &= ~destinationMask) !=
                    board.getBitBoard().getPieceMask(pack.enemyKnightType))
                {
                    move.setCapturedPiece(pack.enemyKnightType);
                    move.setFlag(Board::Move::Flags::CAPTURE);
                    goto CAPTURE_FOUND;
                }
                if ((newBoard.getBitBoard().getPieceMask(pack.enemyBishopType) &= ~destinationMask) !=
                    board.getBitBoard().getPieceMask(pack.enemyBishopType))
                {
                    move.setCapturedPiece(pack.enemyBishopType);
                    move.setFlag(Board::Move::Flags::CAPTURE);
                    goto CAPTURE_FOUND;
                }
                if ((newBoard.getBitBoard().getPieceMask(pack.enemyRookType) &= ~destinationMask) !=
                    board.getBitBoard().getPieceMask(pack.enemyRookType))
                {
                    move.setCapturedPiece(pack.enemyRookType);
                    move.setFlag(Board::Move::Flags::CAPTURE);
                    goto CAPTURE_FOUND;
                }
                if ((newBoard.getBitBoard().getPieceMask(pack.enemyQueenType) &= ~destinationMask) !=
                    board.getBitBoard().getPieceMask(pack.enemyQueenType))
                {
                    move.setCapturedPiece(pack.enemyQueenType);
                    move.setFlag(Board::Move::Flags::CAPTURE);
                    goto CAPTURE_FOUND;
                }
                
            CAPTURE_FOUND:

                newBoard.getLastMove() = move;
                //check if enemy under attack, can happen if a king moves and exposes enemy king to a sliding piece
                if (enemyChecked(newBoard, pack, enemyPawnAttacksCalculator))
                    newBoard.getFlags().set(pack.enemyCheckFlag);
                //check if friendly under attack and that king doesn't attack enemy king (it will be put in check in that case)
                if (!friendlyChecked(newBoard, pack, friendlyPawnAttacksCalculator))
                    nextBoards.push_back(newBoard);

                destinationsMask &= destinationsMask - 1;
            }


            if ((board.getFlags().has(pack.castlingRightsKingSideFlag) &&
                (empty & pack.kingSideCastlingPathMask) == pack.kingSideCastlingPathMask))
            {
                //check is shoved in here because its cheaper to check flags first
                //cant castle if under attack
                if (friendlyChecked(board, pack, friendlyPawnAttacksCalculator))
                    return;

                uint64_t path = pack.kingSideCastlingPathMask;

                while (path)
                {
                    int pathSquare = std::countr_zero(path);
                    uint64_t pathMask = 1ULL << pathSquare;
                    if (isSquareUnderAttack(board, pathMask, pathSquare,
                        board.getBitBoard().getPieceMask(pack.enemyPawnType),
                        board.getBitBoard().getPieceMask(pack.enemyKnightType),
                        board.getBitBoard().getPieceMask(pack.enemyBishopType),
                        board.getBitBoard().getPieceMask(pack.enemyRookType),
                        board.getBitBoard().getPieceMask(pack.enemyQueenType),
                        board.getBitBoard().getPieceMask(pack.enemyKingType),
                        friendlyPawnAttacksCalculator))
                        goto PATH_UNDER_ATTACK;
                    path &= path - 1;
                }

                Board newBoard = board;
                newBoard.getFlags().clear(pack.castlingRightsKingSideFlag);
                newBoard.getFlags().clear(pack.friendlyCheckFlag);
                newBoard.getEnPassantMask() = 0;

                newBoard.getBitBoard().getPieceMask(pack.friendlyKingType) &= ~sourceMask;
                newBoard.getBitBoard().getPieceMask(pack.friendlyKingType) |= pack.kingSideCastlingKingEndMask;
                newBoard.getBitBoard().getPieceMask(pack.friendlyRookType) &= ~pack.kingSideCastlingRookStartMask;
                newBoard.getBitBoard().getPieceMask(pack.friendlyRookType) |= pack.kingSideCastlingRookEndMask;

                Board::Move move(std::countr_zero(sourceMask),
                    std::countr_zero(pack.kingSideCastlingKingEndMask),
                    pack.friendlyKingType, Board::Move::Flags::KING_CASTLE);

                newBoard.getLastMove() = move;

                //check if enemy under attack, can happen if a king moves and exposes enemy king to a sliding piece
                if (enemyChecked(newBoard, pack, enemyPawnAttacksCalculator))
                    newBoard.getFlags().set(pack.enemyCheckFlag);
                //check if friendly under attack and that king doesn't attack enemy king (it will be put in check in that case)
                if (!friendlyChecked(newBoard, pack, friendlyPawnAttacksCalculator))
                    nextBoards.push_back(newBoard);
            }

            PATH_UNDER_ATTACK:

            if ((board.getFlags().has(pack.castlingRightsQueenSideFlag) &&
                (empty & pack.queenSideCastlingPathMask) == pack.queenSideCastlingPathMask))
            {
                //cant castle if under attack
                if (friendlyChecked(board, pack, friendlyPawnAttacksCalculator))
                    return;

                uint64_t path = pack.queenSideCastlingPathMask;

                while (path)
                {
                    int pathSquare = std::countr_zero(path);
                    uint64_t pathMask = 1ULL << pathSquare;
                    if (isSquareUnderAttack(board, pathMask, pathSquare,
                        board.getBitBoard().getPieceMask(pack.enemyPawnType),
                        board.getBitBoard().getPieceMask(pack.enemyKnightType),
                        board.getBitBoard().getPieceMask(pack.enemyBishopType),
                        board.getBitBoard().getPieceMask(pack.enemyRookType),
                        board.getBitBoard().getPieceMask(pack.enemyQueenType),
                        board.getBitBoard().getPieceMask(pack.enemyKingType),
                        friendlyPawnAttacksCalculator))
                        return;
                    path &= path - 1;
                }

                Board newBoard = board;
                newBoard.getFlags().clear(pack.castlingRightsQueenSideFlag);
                newBoard.getFlags().clear(pack.friendlyCheckFlag);
                newBoard.getEnPassantMask() = 0;

                newBoard.getBitBoard().getPieceMask(pack.friendlyKingType) &= ~sourceMask;
                newBoard.getBitBoard().getPieceMask(pack.friendlyKingType) |= pack.queenSideCastlingKingEndMask;
                newBoard.getBitBoard().getPieceMask(pack.friendlyRookType) &= ~pack.queenSideCastlingRookStartMask;
                newBoard.getBitBoard().getPieceMask(pack.friendlyRookType) |= pack.queenSideCastlingRookEndMask;

                Board::Move move(std::countr_zero(sourceMask),
                    std::countr_zero(pack.queenSideCastlingKingEndMask),
                    pack.friendlyKingType, Board::Move::Flags::QUEEN_CASTLE);

                newBoard.getLastMove() = move;

                //check if enemy under attack, can happen if a king moves and exposes enemy king to a sliding piece
                if (enemyChecked(newBoard, pack, enemyPawnAttacksCalculator))
                    newBoard.getFlags().set(pack.enemyCheckFlag);
                //check if friendly under attack and that king doesn't attack enemy king (it will be put in check in that case)
                if (!friendlyChecked(newBoard, pack, friendlyPawnAttacksCalculator))
                    nextBoards.push_back(newBoard);
            }
        }
    };
}