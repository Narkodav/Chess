#pragma once
#include "Common.h"

namespace Chess
{

	struct Piece
	{
        enum class Type : uint8_t
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

        Type piece;
        uint16_t moveCounter = 0;
        uint16_t lastMoved = 0;
        // 2 bytes in case there are custom setups or rules, 
        // the longest real chess game was calculated to be 5949 moves

        bool isEmpty() const { return piece == Type::EMPTY; }
        bool isWhite() const { return piece <= Type::WHITE_KING; }
        bool isBlack() const { return piece >= Type::BLACK_PAWN; }
        bool isPawn() const { return piece == Type::WHITE_PAWN || piece == Type::BLACK_PAWN; }

        Piece() : piece(Type::EMPTY), moveCounter(0), lastMoved(0) {};
        Piece(Type piece) : piece(piece), moveCounter(0), lastMoved(0) {};
        Piece(Type piece, uint16_t moveCounter, uint16_t lastMoved) :
            piece(piece), moveCounter(moveCounter), lastMoved(lastMoved) {};

        Piece(const Piece&) = default;
        Piece(Piece&&) noexcept = default;

        Piece& operator=(const Piece&) = default;
        Piece& operator=(Piece&&) noexcept = default;
	};

    struct Move {
        // For rendering and move validation
        glm::ivec2 from;
        glm::ivec2 to;

        // All pieces that changed position in this move
        std::vector<std::pair<glm::ivec2, Piece>> removedPieces; //tiles set to empty
        std::vector<std::pair<glm::ivec2, Piece>> addedPieces; //tiles set to data in stored piece

        // Move number in the game for history
        uint16_t moveNumber = 0;

        // Optional: evaluation/score for AI
        float score = 0.0f;

        Move() : from(glm::ivec2(0)), to(glm::ivec2(0)), removedPieces(),
            addedPieces(), moveNumber(0), score(0.0f) {};

        Move(const Move&) = default;
        Move& operator=(const Move&) = default;

        Move(Move&&) noexcept = default;
        Move& operator=(Move&&) noexcept = default;
    };

	class Board
	{

    private:
        Utils::ArrayNd<Piece, 8, 8> m_board;
        size_t m_totalMoveCount = 0;
        Move m_lastMove;
        
        bool m_isWhiteChecked = false;
        bool m_isBlackChecked = false;

        glm::ivec2 m_whiteKingPos;
        glm::ivec2 m_blackKingPos;

    public:
        Board() : m_board(), m_totalMoveCount(0), m_lastMove(),
            m_isWhiteChecked(false), m_isBlackChecked(false) {};
        Board(const Board& other) {
            for (int x = 0; x < 8; x++)
                for (int y = 0; y < 8; y++)
                    m_board.at(x, y) = other.m_board.at(x, y);
            m_totalMoveCount = other.m_totalMoveCount;
            m_lastMove = other.m_lastMove;
            m_isWhiteChecked = other.m_isWhiteChecked;
            m_isBlackChecked = other.m_isBlackChecked;
            m_whiteKingPos = other.m_whiteKingPos;
            m_blackKingPos = other.m_blackKingPos;
        }

        Board& operator=(const Board& other) {
            if (this == &other)
                return *this;
            for (int x = 0; x < 8; x++)
                for (int y = 0; y < 8; y++)
                    m_board.at(x, y) = other.m_board.at(x, y);
            m_totalMoveCount = other.m_totalMoveCount;
            m_lastMove = other.m_lastMove;
            m_isWhiteChecked = other.m_isWhiteChecked;
            m_isBlackChecked = other.m_isBlackChecked;
            m_whiteKingPos = other.m_whiteKingPos;
            m_blackKingPos = other.m_blackKingPos;
            return *this;
        }

        Board(Board&&) noexcept = default;
        Board& operator=(Board&&) noexcept = default;

        void incrementMoveCount() { m_totalMoveCount++; };
        size_t getTotalMoveCount() const { return m_totalMoveCount; };

        void setLastMove(Move&& move) { m_lastMove = std::move(move); };
        const Move& getLastMove() const { return m_lastMove; };

        const glm::ivec2& getWhiteKingPos() const { return m_whiteKingPos; };
        const glm::ivec2& getBlackKingPos() const { return m_blackKingPos; };

        void move(const Move& move);

        void reset()
        {

            // Set empty squares in the middle
            for (int i = 0; i < 8; i++) {
                for (int j = 2; j < 6; j++) {
                    m_board.at(i, j) = Piece::Type::EMPTY;
                }
            }

            m_board.at(0, 0) = Piece::Type::WHITE_ROOK;
            m_board.at(1, 0) = Piece::Type::WHITE_KNIGHT;
            m_board.at(2, 0) = Piece::Type::WHITE_BISHOP;
            m_board.at(3, 0) = Piece::Type::WHITE_QUEEN;
            m_board.at(4, 0) = Piece::Type::WHITE_KING;
            m_board.at(5, 0) = Piece::Type::WHITE_BISHOP;
            m_board.at(6, 0) = Piece::Type::WHITE_KNIGHT;
            m_board.at(7, 0) = Piece::Type::WHITE_ROOK;

            for (int i = 0; i < 8; i++) {
                m_board.at(i, 1) = Piece::Type::WHITE_PAWN;
            }

            m_board.at(0, 7) = Piece::Type::BLACK_ROOK;
            m_board.at(1, 7) = Piece::Type::BLACK_KNIGHT;
            m_board.at(2, 7) = Piece::Type::BLACK_BISHOP;
            m_board.at(3, 7) = Piece::Type::BLACK_QUEEN;
            m_board.at(4, 7) = Piece::Type::BLACK_KING;
            m_board.at(5, 7) = Piece::Type::BLACK_BISHOP;
            m_board.at(6, 7) = Piece::Type::BLACK_KNIGHT;
            m_board.at(7, 7) = Piece::Type::BLACK_ROOK;

            for (int i = 0; i < 8; i++) {
                m_board.at(i, 6) = Piece::Type::BLACK_PAWN;
            }

            m_whiteKingPos = glm::ivec2(4, 0);
            m_blackKingPos = glm::ivec2(4, 7);

            m_totalMoveCount = 0;
            m_isWhiteChecked = false;
            m_isBlackChecked = false;
        }

        bool getWhiteChecked() const { return m_isWhiteChecked; };
        bool getBlackChecked() const { return m_isBlackChecked; };

        Piece& at(size_t x, size_t y) { return m_board.at(x, y); };
        const Piece& at(size_t x, size_t y) const { return m_board.at(x, y); };

        //vector variants
        Piece& at(const glm::ivec2& pos) { return m_board.at(pos.x, pos.y); };
        const Piece& at(const glm::ivec2& pos) const { return m_board.at(pos.x, pos.y); };
	};

	// implements the rules of chess, takes a board and returns either possible moves or possible next boards
    // the white pieces are always at the bottom, rotate the array manually for visual representation
    // the bottom left corner is 0,0 and top right is 7,7
    class Calculator
    {
    public:
        // Board orientation constants
        static inline const glm::ivec2 BOTTOM_LEFT  = glm::ivec2( 0,  0);
        static inline const glm::ivec2 TOP_RIGHT    = glm::ivec2( 7,  7);
        static inline const glm::ivec2 LEFT         = glm::ivec2(-1,  0);
        static inline const glm::ivec2 RIGHT        = glm::ivec2( 1,  0);
        static inline const glm::ivec2 UP           = glm::ivec2( 0,  1);
        static inline const glm::ivec2 DOWN         = glm::ivec2( 0, -1);

        // Pawn movement directions
        static inline const glm::ivec2 PAWN_DIRECTION_WHITE = glm::ivec2(0, 1);
        static inline const glm::ivec2 PAWN_DIRECTION_BLACK = glm::ivec2(0, -1);

        static inline const size_t WHITE_EN_PASSANT_RANK = 4;
        static inline const size_t BLACK_EN_PASSANT_RANK = 3;

        static inline const size_t WHITE_PROMOTION_RANK = 7;
        static inline const size_t BLACK_PROMOTION_RANK = 0;

        // Pawn capture directions
        static inline const glm::ivec2 PAWN_CAPTURE_WHITE_LEFT = glm::ivec2(-1, 1);
        static inline const glm::ivec2 PAWN_CAPTURE_WHITE_RIGHT = glm::ivec2(1, 1);

        static inline const glm::ivec2 PAWN_CAPTURE_BLACK_LEFT = glm::ivec2(-1, -1);
        static inline const glm::ivec2 PAWN_CAPTURE_BLACK_RIGHT = glm::ivec2(1, -1);

        // Knight move offsets
        static inline const std::array<glm::ivec2, 8> KNIGHT_MOVES = {
            glm::ivec2(1, 2), glm::ivec2(-1, 2),    //FORWARD
            glm::ivec2(1, -2), glm::ivec2(-1, -2),  //BACKWARD
            glm::ivec2(2, 1), glm::ivec2(2, -1),    //RIGHT
            glm::ivec2(-2, 1), glm::ivec2(-2, -1)   //LEFT
        };

        // Diagonal directions (bishop/queen)
        static inline const std::array<glm::ivec2, 4> DIAGONAL_DIRECTIONS = {
            glm::ivec2(1, 1), glm::ivec2(1, -1),
            glm::ivec2(-1, -1), glm::ivec2(-1, 1)
        };

        // Straight directions (rook/queen)
        static inline const std::array<glm::ivec2, 4> STRAIGHT_DIRECTIONS = {
            glm::ivec2(0, 1), glm::ivec2(1, 0),
            glm::ivec2(0, -1), glm::ivec2(-1, 0)
        };

        static inline const size_t MAXIMUM_CONSERVATIVE_MOVE_AMOUNT = 50; //may be higher but its unlikely
    public:
        //for a specific piece
        static inline std::vector<Move> getAllPossibleMoves(const Board& board, const glm::ivec2& from) {
            return moveFuncs[static_cast<size_t>(board.at(from).piece)](board, from);
        }

        //for all pieces on the board
        static std::vector<Move> getAllPossibleWhiteMoves(const Board& board) {
            std::vector<Move> moves;
            moves.reserve(MAXIMUM_CONSERVATIVE_MOVE_AMOUNT);
            for (int x = 0; x < 8; x++)
                for (int y = 0; y < 8; y++)
                {
                    if (board.at(glm::ivec2(x, y)).isBlack() || board.at(glm::ivec2(x, y)).isEmpty())
                        continue;
                    std::vector<Move> buff = getAllPossibleMoves(board, glm::ivec2(x, y));
                    if (!buff.empty())
                    {
                        for (auto it = buff.begin(); it != buff.end();) {
                            Board nextBoard = board;
                            nextBoard.move(*it);
                            if (nextBoard.getWhiteChecked()) {
                                it = buff.erase(it);
                            }
                            else {
                                ++it;
                            }

                        }
                        if (!buff.empty())
                            moves.insert(moves.end(), buff.begin(), buff.end());
                    }
                }
            return moves;
        }

        static std::vector<Move> getAllPossibleBlackMoves(const Board& board) {
            std::vector<Move> moves;
            moves.reserve(MAXIMUM_CONSERVATIVE_MOVE_AMOUNT);
            for (int x = 0; x < 8; x++)
                for (int y = 0; y < 8; y++)
                {
                    if (board.at(glm::ivec2(x, y)).isWhite())
                        continue;
                    std::vector<Move> buff = getAllPossibleMoves(board, glm::ivec2(x, y));
                    if (!buff.empty())
                    {
                        for (auto it = buff.begin(); it != buff.end();) {
                            Board nextBoard = board;
                            nextBoard.move(*it);
                            if (nextBoard.getBlackChecked()) {
                                it = buff.erase(it);
                            }
                            else {
                                ++it;
                            }
                        }
                        if (!buff.empty())
                            moves.insert(moves.end(), buff.begin(), buff.end());
                    }
                }
            return moves;
        }

        //stores all the moves in a map with coord as key
        static std::unordered_map<glm::ivec2, std::vector<Move>> getAllPossibleWhiteMovesMap(const Board& board) {
            std::unordered_map<glm::ivec2, std::vector<Move>> moves;

            moves.reserve(MAXIMUM_CONSERVATIVE_MOVE_AMOUNT);
            for (int x = 0; x < 8; x++)
                for (int y = 0; y < 8; y++)
                {
                    if (board.at(glm::ivec2(x, y)).isBlack() || board.at(glm::ivec2(x, y)).isEmpty())
                        continue;
                    std::vector<Move> buff = getAllPossibleMoves(board, glm::ivec2(x, y));
                    if (!buff.empty())
                    {
                        for (auto it = buff.begin(); it != buff.end();) {
                            Board nextBoard = board;
                            nextBoard.move(*it);
                            if (nextBoard.getWhiteChecked()) {
                                it = buff.erase(it);
                            }
                            else {
                                ++it;
                            }

                        }
                        if (!buff.empty())
                            moves[glm::ivec2(x, y)] = std::move(buff);
                    }
                }
            return moves;
        }

        //stores all the moves in a map with coord as key
        static std::unordered_map<glm::ivec2, std::vector<Move>> getAllPossibleBlackMovesMap(const Board& board) {
            std::unordered_map<glm::ivec2, std::vector<Move>> moves;

            moves.reserve(MAXIMUM_CONSERVATIVE_MOVE_AMOUNT);
            for (int x = 0; x < 8; x++)
                for (int y = 0; y < 8; y++)
                {
                    if (board.at(glm::ivec2(x, y)).isWhite())
                        continue;
                    std::vector<Move> buff = getAllPossibleMoves(board, glm::ivec2(x, y));
                    if (!buff.empty())
                    {
                        for (auto it = buff.begin(); it != buff.end();) {
                            Board nextBoard = board;
                            nextBoard.move(*it);
                            if (nextBoard.getBlackChecked()) {
                                it = buff.erase(it);
                            }
                            else {
                                ++it;
                            }
                        }
                        if (!buff.empty())
                            moves[glm::ivec2(x, y)] = std::move(buff);
                    }
                }
            return moves;
        }

        //static std::vector<Move> getAllPossibleMoves(const Board& board) {
        //    std::vector<Move> moves;
        //    moves.reserve(MAXIMUM_CONSERVATIVE_MOVE_AMOUNT);
        //    for (int x = 0; x < 8; x++)
        //        for (int y = 0; y < 8; y++)
        //        {
        //            std::vector<Move> buff = getAllPossibleMoves(board, glm::ivec2(x, y));
        //            if (buff.size() > 0)
        //                moves.insert(buff.begin(), buff.end(), moves.end());
        //        }
        //    return moves;
        //}

        //static std::vector<Board> getAllPossibleBoards(const Board& board) {
        //    std::vector<Board> boards;
        //    auto moves = getAllPossibleMoves(board);
        //    boards.reserve(moves.size());

        //    for (auto& move : moves)
        //    {
        //        boards.push_back(board);
        //        boards.back().move(move);
        //        boards.back().incrementMoveCount();
        //    }

        //    return boards;
        //}

        static inline bool isCellUnderAttackWhite(const Board& board, const glm::ivec2& pos);
        static inline bool isCellUnderAttackBlack(const Board& board, const glm::ivec2& pos);

    private:

        static std::vector<Move> getWhitePawnMoves(const Board& board, const glm::ivec2& from);
        static std::vector<Move> getBlackPawnMoves(const Board& board, const glm::ivec2& from);

        static std::vector<Move> getWhiteKnightMoves(const Board& board, const glm::ivec2& from);
        static std::vector<Move> getBlackKnightMoves(const Board& board, const glm::ivec2& from);

        static std::vector<Move> getWhiteBishopMoves(const Board& board, const glm::ivec2& from);
        static std::vector<Move> getBlackBishopMoves(const Board& board, const glm::ivec2& from);

        static std::vector<Move> getWhiteRookMoves(const Board& board, const glm::ivec2& from);
        static std::vector<Move> getBlackRookMoves(const Board& board, const glm::ivec2& from);

        static std::vector<Move> getWhiteQueenMoves(const Board& board, const glm::ivec2& from);
        static std::vector<Move> getBlackQueenMoves(const Board& board, const glm::ivec2& from);

        static std::vector<Move> getWhiteKingMoves(const Board& board, const glm::ivec2& from);
        static std::vector<Move> getBlackKingMoves(const Board& board, const glm::ivec2& from);

        static std::vector<Move> getEmptyMoves(const Board& board, const glm::ivec2& from)
        {
            return std::vector<Move>();
        };

        static inline bool checkBoundLeft   (const glm::ivec2& pos) { return pos.x >= BOTTOM_LEFT.x; };
        static inline bool checkBoundRight  (const glm::ivec2& pos) { return pos.x <= TOP_RIGHT.x;   };
        static inline bool checkBoundBottom (const glm::ivec2& pos) { return pos.y >= BOTTOM_LEFT.y; };
        static inline bool checkBoundTop    (const glm::ivec2& pos) { return pos.y <= TOP_RIGHT.y;   };

        template <typename ColorValidator, typename PositionGenerator>
        static inline void checkDirection(int limit, const Board& board,
            const Piece& currentPiece,
            const glm::ivec2& from, std::vector<Move>& moves, 
            ColorValidator&& friendlyValidator, PositionGenerator&& posGen)
        {
            for (int i = 1; i <= limit; i++)
            {
                auto posNext = posGen(i, from);
                const auto& pieceNext = board.at(posNext);
                if (friendlyValidator(pieceNext))
                    break;
                Move move;
                move.from = from;
                move.to = posNext;
                move.removedPieces.push_back(std::make_pair(from, currentPiece));
                move.addedPieces.push_back(std::make_pair(posNext,
                    Piece(currentPiece.piece, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                if (!pieceNext.isEmpty())
                {
                    move.removedPieces.push_back(std::make_pair(posNext, pieceNext));
                    moves.push_back(move);
                    break;
                }
                moves.push_back(move);
            }
        }

        template <typename ColorValidator, typename PosGenerator, typename EnemyDetector>
        static inline bool checkDirectionForEnemy(const Board& board, const glm::ivec2& targetPos,
            int limit, ColorValidator&& friendlyValidator, PosGenerator&& posGenerator, EnemyDetector&& enemy)
        {
            for (int i = 1; i <= limit; i++)
            {
                auto posNext = posGenerator(i, targetPos);
                const auto& pieceNext = board.at(posNext);

                if (pieceNext.isEmpty())
                    continue;

                if (friendlyValidator(pieceNext))
                    break;

                if (enemy(pieceNext, i))
                    return true;
                else break;
                
            }
            return false;
        }

    private:
        using MoveFunc = std::vector<Move>(*)(const Board&, const glm::ivec2&);
        static constexpr std::array<MoveFunc, static_cast<size_t>(Piece::Type::NUM)> moveFuncs = {
            &getEmptyMoves,             // EMPTY
            &getWhitePawnMoves,         // WHITE_PAWN
            &getWhiteKnightMoves,       // WHITE_KNIGHT
            &getWhiteBishopMoves,       // WHITE_BISHOP
            &getWhiteRookMoves,         // WHITE_ROOK
            &getWhiteQueenMoves,        // WHITE_QUEEN
            &getWhiteKingMoves,         // WHITE_KING
            &getBlackPawnMoves,         // BLACK_PAWN
            &getBlackKnightMoves,       // BLACK_KNIGHT
            &getBlackBishopMoves,       // BLACK_BISHOP
            &getBlackRookMoves,         // BLACK_ROOK
            &getBlackQueenMoves,        // BLACK_QUEEN
            &getBlackKingMoves          // BLACK_KING
        };
    };
}