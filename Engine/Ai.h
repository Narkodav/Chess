#pragma once
#include "Chess.h"

namespace Chess
{
    class Ai {
    public:
        // Basic piece values for material evaluation:
        static inline const std::array<int, static_cast<size_t>(Piece::Type::NUM)> pieceValues = {
            0,      //EMPTY
            100,    //WHITE_PAWN
            300,    //WHITE_KNIGHT
            300,    //WHITE_BISHOP
            500,    //WHITE_ROOK
            900,    //WHITE_QUEEN
            0,      //WHITE_KING //kings are always on the board, no need to count them

            -100,   //BLACK_PAWN
            -300,   //BLACK_KNIGHT
            -300,   //BLACK_BISHOP
            -500,   //BLACK_ROOK
            -900,   //BLACK_QUEEN
            0       //BLACK_KING
        };

        // Piece-square tables (from white's perspective)
        static inline const std::array<int, 64> emptyTable = {
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,
        };

        static inline const std::array<int, 64> whitePawnTable = {
            0,  0,  0,  0,  0,  0,  0,  0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5,  5, 10, 25, 25, 10,  5,  5,
            0,  0,  0, 20, 20,  0,  0,  0,
            5, -5,-10,  0,  0,-10, -5,  5,
            5, 10, 10,-20,-20, 10, 10,  5,
            0,  0,  0,  0,  0,  0,  0,  0
        };

        static inline const std::array<int, 64> whiteKnightTable = {
            -50,-40,-30,-30,-30,-30,-40,-50,
            -40,-20,  0,  0,  0,  0,-20,-40,
            -30,  0, 10, 15, 15, 10,  0,-30,
            -30,  5, 15, 20, 20, 15,  5,-30,
            -30,  0, 15, 20, 20, 15,  0,-30,
            -30,  5, 10, 15, 15, 10,  5,-30,
            -40,-20,  0,  5,  5,  0,-20,-40,
            -50,-40,-30,-30,-30,-30,-40,-50
        };

        static inline const std::array<int, 64> whiteBishopTable = {
            -20,-10,-10,-10,-10,-10,-10,-20,
            -10,  0,  0,  0,  0,  0,  0,-10,
            -10,  0,  5, 10, 10,  5,  0,-10,
            -10,  5,  5, 10, 10,  5,  5,-10,
            -10,  0, 10, 10, 10, 10,  0,-10,
            -10, 10, 10, 10, 10, 10, 10,-10,
            -10,  5,  0,  0,  0,  0,  5,-10,
            -20,-10,-10,-10,-10,-10,-10,-20
        };

        static inline const std::array<int, 64> whiteRookTable = {
            0,  0,  0,  0,  0,  0,  0,  0,
            5, 10, 10, 10, 10, 10, 10,  5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            0,  0,  0,  5,  5,  0,  0,  0
        };

        static inline const std::array<int, 64> whiteQueenTable = {
            -20,-10,-10, -5, -5,-10,-10,-20,
            -10,  0,  0,  0,  0,  0,  0,-10,
            -10,  0,  5,  5,  5,  5,  0,-10,
            -5,  0,  5,  5,  5,  5,  0, -5,
            0,  0,  5,  5,  5,  5,  0, -5,
            -10,  5,  5,  5,  5,  5,  0,-10,
            -10,  0,  5,  0,  0,  0,  0,-10,
            -20,-10,-10, -5, -5,-10,-10,-20
        };

        static inline const std::array<int, 64> whiteKingTable = {
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -20,-30,-30,-40,-40,-30,-30,-20,
            -10,-20,-20,-20,-20,-20,-20,-10,
            20, 20,  0,  0,  0,  0, 20, 20,
            20, 30, 10,  0,  0, 10, 30, 20
        };

        static inline const std::array<int, 64> blackPawnTable = {
            0,  0,  0,  0,  0,  0,  0,  0,
            -5,-10,-10, 20, 20,-10,-10, -5,
            -5,  5, 10,  0,  0, 10,  5, -5,
            0,  0,  0,-20,-20,  0,  0,  0,
            -5, -5,-10,-25,-25,-10, -5, -5,
            -10,-10,-20,-30,-30,-20,-10,-10,
            -50,-50,-50,-50,-50,-50,-50,-50,
            0,  0,  0,  0,  0,  0,  0,  0
        };

        static inline const std::array<int, 64> blackKnightTable = {
            50, 40, 30, 30, 30, 30, 40, 50,
            40, 20,  0, -5, -5,  0, 20, 40,
            30, -5,-10,-15,-15,-10, -5, 30,
            30,  0,-15,-20,-20,-15,  0, 30,
            30, -5,-15,-20,-20,-15, -5, 30,
            30,  0,-10,-15,-15,-10,  0, 30,
            40, 20,  0,  0,  0,  0, 20, 40,
            50, 40, 30, 30, 30, 30, 40, 50
        };

        static inline const std::array<int, 64> blackBishopTable = {
            20, 10, 10, 10, 10, 10, 10, 20,
            10, -5,  0,  0,  0,  0, -5, 10,
            10,-10,-10,-10,-10,-10,-10, 10,
            10,  0,-10,-10,-10,-10,  0, 10,
            10, -5, -5,-10,-10, -5, -5, 10,
            10,  0, -5,-10,-10, -5,  0, 10,
            10,  0,  0,  0,  0,  0,  0, 10,
            20, 10, 10, 10, 10, 10, 10, 20
        };

        static inline const std::array<int, 64> blackRookTable = {
            0,  0,  0, -5, -5,  0,  0,  0,
            5,  0,  0,  0,  0,  0,  0,  5,
            5,  0,  0,  0,  0,  0,  0,  5,
            5,  0,  0,  0,  0,  0,  0,  5,
            5,  0,  0,  0,  0,  0,  0,  5,
            5,  0,  0,  0,  0,  0,  0,  5,
            -5,-10,-10,-10,-10,-10,-10, -5,
            0,  0,  0,  0,  0,  0,  0,  0
        };

        static inline const std::array<int, 64> blackQueenTable = {
            20, 10, 10,  5,  5, 10, 10, 20,
            10,  0, -5,  0,  0,  0,  0, 10,
            10, -5, -5, -5, -5, -5,  0, 10,
            0,  0, -5, -5, -5, -5,  0,  5,
            5,  0, -5, -5, -5, -5,  0,  5,
            10,  0, -5, -5, -5, -5,  0, 10,
            10,  0,  0,  0,  0,  0,  0, 10,
            20, 10, 10,  5,  5, 10, 10, 20
        };

        static inline const std::array<int, 64> blackKingTable = {
            -20,-30,-10,  0,  0,-10,-30,-20,
            -20,-20,  0,  0,  0,  0,-20,-20,
            10, 20, 20, 20, 20, 20, 20, 10,
            20, 30, 30, 40, 40, 30, 30, 20,
            30, 40, 40, 50, 50, 40, 40, 30,
            30, 40, 40, 50, 50, 40, 40, 30,
            30, 40, 40, 50, 50, 40, 40, 30,
            30, 40, 40, 50, 50, 40, 40, 30
        };

        static inline const std::array<std::array<int, 64>,
            static_cast<size_t>(Piece::Type::NUM)> pieceSquareTables = {
                emptyTable,         //EMPTY
                whitePawnTable,     //WHITE_PAWN
                whiteKnightTable,   //WHITE_KNIGHT
                whiteBishopTable,   //WHITE_BISHOP
                whiteRookTable,     //WHITE_ROOK
                whiteQueenTable,    //WHITE_QUEEN
                whiteKingTable,     //WHITE_KING
                blackPawnTable,     //BLACK_PAWN
                blackKnightTable,   //BLACK_KNIGHT
                blackBishopTable,   //BLACK_BISHOP
                blackRookTable,     //BLACK_ROOK
                blackQueenTable,    //BLACK_QUEEN
                blackKingTable      //BLACK_KING
        };

    private:
        size_t m_searchDepth;
        bool m_isWhite; // Which side the AI plays

    public:

        void reset(bool playingWhite, size_t searchDepth = 4)
        {
            m_isWhite = playingWhite;
            m_searchDepth = searchDepth;
        }

        Move getBestMove(const Chess::Board& board) {
            auto possibleMoves = m_isWhite ?
                Chess::Calculator::getAllPossibleWhiteMoves(board) :  // Using vector version
                Chess::Calculator::getAllPossibleBlackMoves(board);   // Using vector version

            sortMoves(possibleMoves, board);
            Move bestMove;
            int bestScore = INT_MIN;

            for (const auto& move : possibleMoves) {
                Chess::Board nextBoard = board;
                nextBoard.move(move);

                int score = -minimax(nextBoard, m_searchDepth - 1,
                    !m_isWhite, -INT_MAX, -bestScore);

                if (score > bestScore) {
                    bestScore = score;
                    bestMove = move;
                }
            }
            return bestMove;
        }

    private:
        int minimax(Chess::Board& board, int depth, bool isWhite,
            int alpha, int beta) {
            if (depth == 0)
                return evaluatePosition(board, isWhite);

            auto possibleMoves = isWhite ?
                Chess::Calculator::getAllPossibleWhiteMoves(board) :  // Using vector version
                Chess::Calculator::getAllPossibleBlackMoves(board);   // Using vector version

            if (possibleMoves.empty()) {
                // Checkmate check
                if (m_isWhite)
                {
                    if (board.getBlackChecked())
                        return 20000;
                    else if (board.getWhiteChecked())
                        return -20000;
                }
                else
                {
                    if (board.getBlackChecked())
                        return -20000;
                    else if (board.getWhiteChecked())
                        return 20000;
                }
                return 0; // Stalemate
            }

            sortMoves(possibleMoves, board);

            int bestScore = -INT_MAX;

            for (const auto& move : possibleMoves) {  // Simpler iteration
                Chess::Board nextBoard = board;
                nextBoard.move(move);

                int score = -minimax(nextBoard, depth - 1, !isWhite,
                    -beta, -alpha);

                bestScore = std::max(bestScore, score);
                alpha = std::max(alpha, score);

                if (alpha >= beta)
                    return bestScore; // Beta cutoff
            }

            return bestScore;
        }

        // Add move scoring function
        int scoreMoveForOrdering(const Move& move, const Chess::Board& board) {
            constexpr int CAPTURE_BONUS = 10000;  // Base score for captures

            int score = 0;

            // If it's a capture, score using MVV-LVA
            if (move.isCapture()) {
                // Victim value - Attacker value (MVV-LVA)
                int victimValue = std::abs(pieceValues[static_cast<size_t>(board.at(move.to.x, move.to.y).piece)]);
                int attackerValue = std::abs(pieceValues[static_cast<size_t>(board.at(move.from.x, move.from.y).piece)]);
                score = CAPTURE_BONUS + victimValue - (attackerValue / 100);
            }

            // Add positional change score
            int fromSquare = move.from.y * 8 + move.from.x;
            int toSquare = move.to.y * 8 + move.to.x;
            auto pieceType = static_cast<size_t>(board.at(move.from.x, move.from.y).piece);

            // Prefer moves to better squares
            score += (pieceSquareTables[pieceType][toSquare] -
                pieceSquareTables[pieceType][fromSquare]) / 100;

            return score;
        }

        // Add move sorting function
        void sortMoves(std::vector<Move>& moves, const Chess::Board& board) {
            std::sort(moves.begin(), moves.end(),
                [this, &board](const Move& a, const Move& b) {
                    return scoreMoveForOrdering(a, board) > scoreMoveForOrdering(b, board);
                });
        }

        int evaluatePosition(const Chess::Board& board, bool isWhite) {
            int score = 0;

            // Material counting
            // Piece positioning
            // Mobility
            // King safety
            // Pawn structure

            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    const Chess::Piece& piece = board.at(x, y);
                    if (piece.isEmpty())
                        continue;

                    // Material value
                    int value = pieceValues[static_cast<size_t>(piece.piece)];
                    score += value;

                    // Positional value - direct lookup from tables
                    int square = y * 8 + x;
                    score += pieceSquareTables[static_cast<size_t>(piece.piece)][square];
                }
            }

            return score;
        }
    };

}