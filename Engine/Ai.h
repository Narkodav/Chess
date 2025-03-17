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
        static inline const std::array<int, static_cast<size_t>(PieceTypes::NUM)> pieceValues = {
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
            static_cast<size_t>(PieceTypes::NUM)> pieceSquareTables = {
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
        void getBestMoveAsync(Board board, MT::ThreadPool& pool, std::function<void(Board)> callback)
        {
            pool.pushTask([this, board = std::move(board), callback]() {
                Board boardNext;
                try
                {
#ifdef _DEBUG
                    m_profiler.timeOperation(std::this_thread::get_id(),
                    "Ai move selection", [this, &boardNext, &board]() {
                        m_pendingTasks++;
                        boardNext = getBestMove(board);
                        m_pendingTasks--;
                        });
                    m_profiler.printStats(std::this_thread::get_id());
                    m_profiler.reset(std::this_thread::get_id());
#else
                    m_pendingTasks++;
                    boardNext = getBestMove(board);
                    m_pendingTasks--;
#endif
                    callback(boardNext);
                }
                catch (std::exception& e)
                {
                    m_pendingTasks--;
                    if (e.what() == "Abort")
                        return;
                }

                });
        }

        Board getBestMove(const Board& board) {

#ifdef _DEBUG
            std::vector<Chess::Board> possibleBoards;
            m_profiler.timeOperation(std::this_thread::get_id(),
                "Possible boards generation", [this, &board, &possibleBoards]() {
                    possibleBoards = m_isWhite ?
                        Calculator::getNextBoardsWhite(board) :
                        Calculator::getNextBoardsBlack(board);
                });

            m_profiler.timeOperation(std::this_thread::get_id(),
                "Board sorting", [this, &board, &possibleBoards]() {
                    sortBoards(possibleBoards, board, m_isWhite);
                });
#else
            auto possibleBoards = m_isWhite ?
                Calculator::getNextBoardsWhite(board) :
                Calculator::getNextBoardsBlack(board);

            sortBoards(possibleBoards, board, m_isWhite);
#endif
            Board bestBoard;
            int bestScore = INT_MIN;

            for (const auto& board : possibleBoards) {
                int score = -minimax(board, m_searchDepth - 1,
                    !m_isWhite, -INT_MAX, -bestScore);

                if (score > bestScore) {
                    bestScore = score;
                    bestBoard = board;
                }
            }
            return bestBoard;
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
                        Calculator::getNextBoardsWhite(board) :
                        Calculator::getNextBoardsBlack(board);
                });
#else
            if (depth == 0)
                return evaluatePosition(board, isWhite);

            auto possibleBoards = isWhite ?
                Calculator::getNextBoardsWhite(board) :
                Calculator::getNextBoardsBlack(board);
#endif

            if (possibleBoards.empty()) {
                // Checkmate check
                if (m_isWhite)
                {
                    if (board.isBlackChecked())
                        return 20000;
                    else if (board.isWhiteChecked())
                        return -20000;
                }
                else
                {
                    if (board.isBlackChecked())
                        return -20000;
                    else if (board.isWhiteChecked())
                        return 20000;
                }
                return 0; // Stalemate
            }

#ifdef _DEBUG
            m_profiler.timeOperation(std::this_thread::get_id(),
                "Board sorting", [this, &board, &isWhite, &possibleBoards]() {
                    sortBoards(possibleBoards, board, isWhite);
                });
#else
            sortBoards(possibleBoards, board, isWhite);
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
        int scoreMoveForOrdering(const Board& nextBoard, const Chess::Board& board, bool isWhite) {
            constexpr int CAPTURE_BONUS = 10000;  // Base score for captures

            int score = 0;
            bool isCapture = isWhite ?
                nextBoard.getBitBoard().getAllBlackPieces() != board.getBitBoard().getAllBlackPieces() :
                nextBoard.getBitBoard().getAllWhitePieces() != board.getBitBoard().getAllWhitePieces();

            std::pair<int, int> fromTo;

            if (isWhite)
                fromTo = Calculator::getFromToPairWhite(nextBoard, board);
            else fromTo = Calculator::getFromToPairBlack(nextBoard, board);

            // If it's a capture, score using MVV-LVA
            auto fromType = board.getPieceTypeAtSquare(fromTo.first);
            if (isCapture) {
                // Victim value - Attacker value (MVV-LVA)
                int victimValue = std::abs(pieceValues[static_cast<size_t>(
                    board.getPieceTypeAtSquare(fromTo.second))]);
                int attackerValue = std::abs(pieceValues[static_cast<size_t>(fromType)]);
                score = CAPTURE_BONUS + victimValue - (attackerValue / 100);
            }

            // Prefer moves to better squares
            score += (pieceSquareTables[static_cast<size_t>(fromType)][fromTo.second] -
                pieceSquareTables[static_cast<size_t>(fromType)][fromTo.first]) / 100;

            return score;
        }

        // Add move sorting function
        void sortBoards(std::vector<Board>& boards, const Chess::Board& board, bool isWhite) {
            std::sort(boards.begin(), boards.end(),
                [this, &board, &isWhite](const Board& a, const Board& b) {
                    //if (a.getBitBoard().getAllPieces() == b.getBitBoard().getAllPieces())
                    //    __debugbreak();
                    //if (a.getBitBoard().getAllPieces() == board.getBitBoard().getAllPieces())
                    //    __debugbreak();
                    //if (board.getBitBoard().getAllPieces() == b.getBitBoard().getAllPieces())
                    //    __debugbreak();
                    return scoreMoveForOrdering(a, board, isWhite) >
                        scoreMoveForOrdering(b, board, isWhite);
                });
        }

        int evaluatePosition(const Chess::Board& board, bool isWhite) {
            int score = 0;
            auto empty = ~board.getBitBoard().getAllPieces();
            // Material counting
            // Piece positioning
            // Mobility
            // King safety
            // Pawn structure

            //white pieces
            score += getPieceScore(board.getBitBoard().whiteBishops, PieceTypes::WHITE_BISHOP);
            score += getPieceScore(board.getBitBoard().whiteKnights, PieceTypes::WHITE_KNIGHT);
            score += getPieceScore(board.getBitBoard().whitePawns, PieceTypes::WHITE_PAWN);
            score += getPieceScore(board.getBitBoard().whiteRooks, PieceTypes::WHITE_ROOK);
            score += getPieceScore(board.getBitBoard().whiteQueens, PieceTypes::WHITE_QUEEN);

            //black pieces
            score += getPieceScore(board.getBitBoard().blackBishops, PieceTypes::BLACK_BISHOP);
            score += getPieceScore(board.getBitBoard().blackKnights, PieceTypes::BLACK_KNIGHT);
            score += getPieceScore(board.getBitBoard().blackPawns, PieceTypes::BLACK_PAWN);
            score += getPieceScore(board.getBitBoard().blackRooks, PieceTypes::BLACK_ROOK);
            score += getPieceScore(board.getBitBoard().blackQueens, PieceTypes::BLACK_QUEEN);
            
            return score;
        }

        inline int getPieceScore(uint64_t pieceMask, PieceTypes type)
        {
            int score = 0;
            while (pieceMask) {
                int square = std::countr_zero(pieceMask);
                score += pieceValues[static_cast<size_t>(type)];
                score += pieceSquareTables[static_cast<size_t>(type)][square];
                pieceMask &= pieceMask - 1;
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