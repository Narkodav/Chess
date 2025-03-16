#pragma once
#include <functional>
#include <atomic>

#include "Chess.h"
#include "Constants.h"
#include "Multithreading/ThreadPool.h"

#ifdef _DEBUG
#include "Profiler.h"
#endif

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
        std::atomic<bool> m_shouldAbort = false;
        std::atomic<bool> m_isPaused{ false };
        std::atomic<size_t> m_pendingTasks;
        std::mutex m_pauseMutex;
        std::condition_variable m_pauseCondition;

#ifdef _DEBUG
        Profiler<std::thread::id> m_profiler;
#endif

    public:

        void abortAndWait()
        {
            m_shouldAbort = true;
            if(m_isPaused)
            {
                m_isPaused = false;
                m_pauseCondition.notify_all();
            }
            while (m_pendingTasks.load() > 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            m_shouldAbort = false;
        }

        void setPaused(bool paused) {
            m_isPaused = paused;
            if (!paused) {
                m_pauseCondition.notify_all(); // Wake up paused calculation
            }
        }

        void reset(bool playingWhite, size_t searchDepth = 4)
        {
            m_shouldAbort = false;
            m_isWhite = playingWhite;
            m_searchDepth = searchDepth;
        }

        //makes a copy of the board for a completely isolated async search, not an expensive operation overall
        void getBestMoveAsync(Board board, MT::ThreadPool& pool, std::function<void(Move)> callback)
        {
            pool.pushTask([this, board = std::move(board), callback]() {
                Move move;
                try
                {
#ifdef _DEBUG
                    m_profiler.timeOperation(std::this_thread::get_id(),
                    "Ai move selection", [this, &move, &board]() {
                        m_pendingTasks++;
                        move = getBestMove(board);
                        m_pendingTasks--;
                        });
                    m_profiler.printStats(std::this_thread::get_id());
                    m_profiler.reset(std::this_thread::get_id());
#else
                    m_pendingTasks++;
                    move = getBestMove(board);
                    m_pendingTasks--;
#endif
                    callback(move);
                }
                catch (std::exception& e)
                {
                    m_pendingTasks--;
                    if (e.what() == "Abort")
                        return;
                }

                });
        }

        Move getBestMove(const Board& board) {

#ifdef _DEBUG
            std::vector<Chess::Board> possibleBoards;
            m_profiler.timeOperation(std::this_thread::get_id(),
                "Possible boards generation", [this, &board, &possibleBoards]() {
                    possibleBoards = m_isWhite ?
                        Calculator::getAllPossibleNextBoardsWhite(board) :
                        Calculator::getAllPossibleNextBoardsBlack(board);
                });

            m_profiler.timeOperation(std::this_thread::get_id(),
                "Board sorting", [this, &board, &possibleBoards]() {
                    sortBoards(possibleBoards, board);
                });
#else
            auto possibleBoards = m_isWhite ?
                Calculator::getAllPossibleNextBoardsWhite(board) :
                Calculator::getAllPossibleNextBoardsBlack(board);

            sortBoards(possibleBoards, board);
#endif
            Move bestMove;
            int bestScore = INT_MIN;

            for (const auto& board : possibleBoards) {
                int score = -minimax(board, m_searchDepth - 1,
                    !m_isWhite, -INT_MAX, -bestScore);

                if (score > bestScore) {
                    bestScore = score;
                    bestMove = board.getLastMove();
                }
            }
            return bestMove;
        }

        size_t getPendingTasks() const
        {
            return m_pendingTasks.load();
        }

    private:
        int minimax(const Chess::Board& board, int depth, bool isWhite,
            int alpha, int beta) {
            runtimeStateChecks();

#ifdef _DEBUG
            if (depth == 0)
            {
                auto scopedTiming = m_profiler.timeOperationScoped(
                    std::this_thread::get_id(), "Position evaluation");
                return evaluatePosition(board, isWhite);
            }

            std::vector<Chess::Board> possibleBoards;
            m_profiler.timeOperation(std::this_thread::get_id(),
                "Possible boards generation", [this, &isWhite, &board, &possibleBoards]() {
                    possibleBoards = isWhite ?
                        Calculator::getAllPossibleNextBoardsWhite(board) :
                        Calculator::getAllPossibleNextBoardsBlack(board);
                });
#else
            if (depth == 0)
                return evaluatePosition(board, isWhite);

            auto possibleBoards = isWhite ?
                Calculator::getAllPossibleNextBoardsWhite(board) :
                Calculator::getAllPossibleNextBoardsBlack(board);
#endif

            if (possibleBoards.empty()) {
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

#ifdef _DEBUG
            m_profiler.timeOperation(std::this_thread::get_id(),
                "Board sorting", [this, &board, &possibleBoards]() {
                    sortBoards(possibleBoards, board);
                });
#else
            sortBoards(possibleBoards, board);
#endif

            int bestScore = -INT_MAX;

            for (const auto& nextBoard : possibleBoards) {
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
        void sortBoards(std::vector<Board>& boards, const Chess::Board& board) {
            std::sort(boards.begin(), boards.end(),
                [this, &board](const Board& a, const Board& b) {
                    return scoreMoveForOrdering(a.getLastMove(), board) >
                        scoreMoveForOrdering(b.getLastMove(), board);
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

        inline void runtimeStateChecks() {
            if (m_isPaused) {
                std::unique_lock<std::mutex> lock(m_pauseMutex);
                while (m_isPaused) {
                    // Wait for 100ms at a time
                    m_pauseCondition.wait_for(lock, std::chrono::milliseconds(100), [this]() {
                        return !m_isPaused;
                        });
                }
            }
            if (m_shouldAbort) {
                throw std::runtime_error("Abort");
            }
        }
    };
}