#include "Chess.h"

namespace Chess
{
    void Board::move(const Move& move) //doesn't itself check if the move is legit
    {
        
        for (auto& piece : move.removedPieces)
            at(piece.first) = Piece::Type::EMPTY;

        for (auto& piece : move.addedPieces)
        {
            if (piece.second.piece == Piece::Type::WHITE_KING)
            {
                m_whiteKingPos = piece.first;
            }
            else if (piece.second.piece == Piece::Type::BLACK_KING)
            {
                m_blackKingPos = piece.first;
            }
            at(piece.first) = piece.second;
        }
        
        if (Calculator::isCellUnderAttackWhite(*this, m_whiteKingPos))
            m_isWhiteChecked = true;
        else m_isWhiteChecked = false;
        if (Calculator::isCellUnderAttackBlack(*this, m_blackKingPos))
            m_isBlackChecked = true;
        else m_isBlackChecked = false;

        m_lastMove = move;
    }

    // checks have a lot of duplicated code due to optimization reasons, need to shorten them in the future
    // this implementation is temporary, will be shortened and optimized

    std::vector<Move> Calculator::getWhitePawnMoves(const Board& board, const glm::ivec2& from)
    {
        // no need to bound check from because its a callers responsibility to ensure it
        std::vector<Move> moves;
        moves.reserve(4);
        const auto& currentPiece = board.at(from);

        if (from.y == WHITE_EN_PASSANT_RANK)
        {
            //En passant check
            auto adjPos = from + LEFT;
            if (checkBoundLeft(adjPos))
            {
                const auto& pieceLeft = board.at(adjPos);
                //no need to check bounds because pawns cannot be on first or last rank
                const auto& pieceLeftFront = board.at(adjPos + PAWN_DIRECTION_WHITE);
                if (pieceLeft.piece == Piece::Type::BLACK_PAWN &&
                    pieceLeft.lastMoved == board.getTotalMoveCount() - 1 &&
                    pieceLeft.moveCounter == 1 &&
                    pieceLeftFront.isEmpty())
                {
                    Move move;
                    move.addedPieces.push_back(std::make_pair(adjPos + PAWN_DIRECTION_WHITE,
                        Piece(Piece::Type::WHITE_PAWN, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(adjPos, pieceLeft));
                    move.from = from;
                    move.to = adjPos + PAWN_DIRECTION_WHITE;
                    moves.push_back(move);
                }
            }

            adjPos = from + RIGHT;
            if (checkBoundRight(adjPos))
            {
                const auto& pieceRight = board.at(adjPos);
                //no need to check bounds because pawns cannot be on first or last rank
                const auto& pieceRightFront = board.at(adjPos + PAWN_DIRECTION_WHITE);
                if (pieceRight.piece == Piece::Type::BLACK_PAWN &&
                    pieceRight.lastMoved == board.getTotalMoveCount() - 1 &&
                    pieceRight.moveCounter == 1 &&
                    pieceRightFront.isEmpty())
                {
                    Move move;
                    move.addedPieces.push_back(std::make_pair(adjPos + PAWN_DIRECTION_WHITE,
                        Piece(Piece::Type::WHITE_PAWN, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(adjPos, pieceRight));
                    move.from = from;
                    move.to = adjPos + PAWN_DIRECTION_WHITE;
                    moves.push_back(move);
                }
            }
        }

        auto nextPos = from + PAWN_DIRECTION_WHITE;
        const auto& pieceNext = board.at(nextPos);

        //regular move, no need to check bounds because pawns cannot be on first or last rank
        if (pieceNext.isEmpty())
        {
            if (nextPos.y == WHITE_PROMOTION_RANK)
            {
                Move move;
                move.addedPieces.push_back(std::make_pair(nextPos,
                    Piece(Piece::Type::WHITE_QUEEN, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                move.removedPieces.push_back(std::make_pair(from, currentPiece));
                move.from = from;
                move.to = nextPos;
                moves.push_back(move);
            }
            else
            {
                Move move;
                move.addedPieces.push_back(std::make_pair(nextPos,
                    Piece(Piece::Type::WHITE_PAWN, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                move.removedPieces.push_back(std::make_pair(from, currentPiece));
                move.from = from;
                move.to = nextPos;
                moves.push_back(move);
                if (currentPiece.moveCounter == 0 && board.at(nextPos + PAWN_DIRECTION_WHITE).isEmpty())
                {
                    Move moveNext;
                    moveNext.addedPieces.push_back(std::make_pair(nextPos + PAWN_DIRECTION_WHITE,
                        Piece(Piece::Type::WHITE_PAWN, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moveNext.removedPieces.push_back(std::make_pair(from, currentPiece));
                    moveNext.from = from;
                    moveNext.to = nextPos + PAWN_DIRECTION_WHITE;
                    moves.push_back(moveNext);
                }
            }
        }

        //regular captures
        auto captureRight = from + PAWN_CAPTURE_WHITE_RIGHT;
        auto captureLeft = from + PAWN_CAPTURE_WHITE_LEFT;
        if (checkBoundRight(captureRight))
        {
            const auto& pieceRight = board.at(captureRight);
            if (!pieceRight.isEmpty() && pieceRight.isBlack())
            {
                Move move;
                move.from = from;
                move.to = captureRight;
                move.removedPieces.push_back(std::make_pair(from, currentPiece));
                move.removedPieces.push_back(std::make_pair(captureRight, pieceRight));
                if (captureRight.y == WHITE_PROMOTION_RANK)
                    move.addedPieces.push_back(std::make_pair(captureRight,
                        Piece(Piece::Type::WHITE_QUEEN, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                else
                    move.addedPieces.push_back(std::make_pair(captureRight,
                        Piece(Piece::Type::WHITE_PAWN, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                moves.push_back(move);
            }
        }
        if (checkBoundLeft(captureLeft))
        {
            const auto& pieceLeft = board.at(captureLeft);
            if (!pieceLeft.isEmpty() && pieceLeft.isBlack())
            {
                Move move;
                move.from = from;
                move.to = captureLeft;
                move.removedPieces.push_back(std::make_pair(from, currentPiece));
                move.removedPieces.push_back(std::make_pair(captureLeft, pieceLeft));
                if (captureLeft.y == WHITE_PROMOTION_RANK)
                    move.addedPieces.push_back(std::make_pair(captureLeft,
                        Piece(Piece::Type::WHITE_QUEEN, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                else
                    move.addedPieces.push_back(std::make_pair(captureLeft,
                        Piece(Piece::Type::WHITE_PAWN, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                moves.push_back(move);
            }
        }

        return moves;
    }

    std::vector<Move> Calculator::getBlackPawnMoves(const Board& board, const glm::ivec2& from)
    {
        // no need to bound check from because its a callers responsibility to ensure it
        std::vector<Move> moves;
        moves.reserve(4);
        const auto& currentPiece = board.at(from);

        if (from.y == BLACK_EN_PASSANT_RANK)
        {
            //En passant check
            auto adjPos = from + LEFT;
            if (checkBoundLeft(adjPos))
            {
                const auto& pieceLeft = board.at(adjPos);
                //no need to check bounds because pawns cannot be on first or last rank
                const auto& pieceLeftFront = board.at(adjPos + PAWN_DIRECTION_BLACK);
                if (pieceLeft.piece == Piece::Type::WHITE_PAWN &&
                    pieceLeft.lastMoved == board.getTotalMoveCount() - 1 &&
                    pieceLeft.moveCounter == 1 &&
                    pieceLeftFront.isEmpty())
                {
                    Move move;
                    move.addedPieces.push_back(std::make_pair(adjPos + PAWN_DIRECTION_BLACK,
                        Piece(Piece::Type::BLACK_PAWN, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(adjPos, pieceLeft));
                    move.from = from;
                    move.to = adjPos + PAWN_DIRECTION_BLACK;
                    moves.push_back(move);
                }
            }

            adjPos = from + RIGHT;
            if (checkBoundRight(adjPos))
            {
                const auto& pieceRight = board.at(adjPos);
                //no need to check bounds because pawns cannot be on first or last rank
                const auto& pieceRightFront = board.at(adjPos + PAWN_DIRECTION_BLACK);
                if (pieceRight.piece == Piece::Type::WHITE_PAWN &&
                    pieceRight.lastMoved == board.getTotalMoveCount() - 1 &&
                    pieceRight.moveCounter == 1 &&
                    pieceRightFront.isEmpty())
                {
                    Move move;
                    move.addedPieces.push_back(std::make_pair(adjPos + PAWN_DIRECTION_BLACK,
                        Piece(Piece::Type::BLACK_PAWN, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(adjPos, pieceRight));
                    move.from = from;
                    move.to = adjPos + PAWN_DIRECTION_BLACK;
                    moves.push_back(move);
                }
            }
        }

        auto nextPos = from + PAWN_DIRECTION_BLACK;
        const auto& pieceNext = board.at(nextPos);

        //regular move, no need to check bounds because pawns cannot be on first or last rank
        if (pieceNext.isEmpty())
        {
            if (nextPos.y == BLACK_PROMOTION_RANK)
            {
                Move move;
                move.addedPieces.push_back(std::make_pair(nextPos,
                    Piece(Piece::Type::BLACK_QUEEN, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                move.removedPieces.push_back(std::make_pair(from, currentPiece));
                move.from = from;
                move.to = nextPos;
                moves.push_back(move);
            }
            else
            {
                Move move;
                move.addedPieces.push_back(std::make_pair(nextPos,
                    Piece(Piece::Type::BLACK_PAWN, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                move.removedPieces.push_back(std::make_pair(from, currentPiece));
                move.from = from;
                move.to = nextPos;
                moves.push_back(move);
                if (currentPiece.moveCounter == 0 && board.at(nextPos + PAWN_DIRECTION_BLACK).isEmpty())
                {
                    Move moveNext;
                    moveNext.addedPieces.push_back(std::make_pair(nextPos + PAWN_DIRECTION_BLACK,
                        Piece(Piece::Type::BLACK_PAWN, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moveNext.removedPieces.push_back(std::make_pair(from, currentPiece));
                    moveNext.from = from;
                    moveNext.to = nextPos + PAWN_DIRECTION_BLACK;
                    moves.push_back(moveNext);
                }
            }
        }

        //regular captures
        auto captureRight = from + PAWN_CAPTURE_BLACK_RIGHT;
        auto captureLeft = from + PAWN_CAPTURE_BLACK_LEFT;
        if (checkBoundRight(captureRight))
        {
            const auto& pieceRight = board.at(captureRight);
            if (!pieceRight.isEmpty() && pieceRight.isWhite())
            {
                Move move;
                move.from = from;
                move.to = captureRight;
                move.removedPieces.push_back(std::make_pair(from, currentPiece));
                move.removedPieces.push_back(std::make_pair(captureRight, pieceRight));
                if (captureRight.y == BLACK_PROMOTION_RANK)
                    move.addedPieces.push_back(std::make_pair(captureRight,
                        Piece(Piece::Type::BLACK_QUEEN, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                else
                    move.addedPieces.push_back(std::make_pair(captureRight,
                        Piece(Piece::Type::BLACK_PAWN, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                moves.push_back(move);
            }
        }
        if (checkBoundLeft(captureLeft))
        {
            const auto& pieceLeft = board.at(captureLeft);
            if (!pieceLeft.isEmpty() && pieceLeft.isWhite())
            {
                Move move;
                move.from = from;
                move.to = captureLeft;
                move.removedPieces.push_back(std::make_pair(from, currentPiece));
                move.removedPieces.push_back(std::make_pair(captureLeft, pieceLeft));
                if (captureLeft.y == BLACK_PROMOTION_RANK)
                    move.addedPieces.push_back(std::make_pair(captureLeft,
                        Piece(Piece::Type::BLACK_QUEEN, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                else
                    move.addedPieces.push_back(std::make_pair(captureLeft,
                        Piece(Piece::Type::BLACK_PAWN, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                moves.push_back(move);
            }
        }

        return moves;
    }


    std::vector<Move> Calculator::getWhiteKnightMoves(const Board& board, const glm::ivec2& from)
    {
        std::vector<Move> moves;
        moves.reserve(8);
        const auto& currentPiece = board.at(from);

        //forward moves
        if (from.y + 2 < 8)
        {
            auto nextPos = from + glm::ivec2(1, 2);
            if (nextPos.x < 8)
            {
                const auto& nextPiece = board.at(nextPos);

                if (nextPiece.isEmpty())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::WHITE_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
                else if (nextPiece.isBlack())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(nextPos, nextPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::WHITE_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }

            }

            nextPos = from + glm::ivec2(-1, 2);
            if (nextPos.x > -1)
            {
                const auto& nextPiece = board.at(nextPos);

                if (nextPiece.isEmpty())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::WHITE_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
                else if (nextPiece.isBlack())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(nextPos, nextPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::WHITE_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
            }
        }

        //backward moves
        if (from.y - 2 > -1)
        {
            auto nextPos = from + glm::ivec2(1, -2);
            if (nextPos.x < 8)
            {
                const auto& nextPiece = board.at(nextPos);

                if (nextPiece.isEmpty())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::WHITE_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
                else if (nextPiece.isBlack())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(nextPos, nextPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::WHITE_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }

            }

            nextPos = from + glm::ivec2(-1, -2);
            if (nextPos.x > -1)
            {
                const auto& nextPiece = board.at(nextPos);

                if (nextPiece.isEmpty())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::WHITE_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
                else if (nextPiece.isBlack())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(nextPos, nextPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::WHITE_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
            }
        }

        //right moves
        if (from.x + 2 < 8)
        {
            auto nextPos = from + glm::ivec2(2, 1);
            if (nextPos.y < 8)
            {
                const auto& nextPiece = board.at(nextPos);

                if (nextPiece.isEmpty())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::WHITE_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
                else if (nextPiece.isBlack())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(nextPos, nextPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::WHITE_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }

            }

            nextPos = from + glm::ivec2(2, -1);
            if (nextPos.y > -1)
            {
                const auto& nextPiece = board.at(nextPos);

                if (nextPiece.isEmpty())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::WHITE_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
                else if (nextPiece.isBlack())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(nextPos, nextPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::WHITE_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
            }
        }

        //left moves
        if (from.x - 2 > -1)
        {
            auto nextPos = from + glm::ivec2(-2, 1);
            if (nextPos.y < 8)
            {
                const auto& nextPiece = board.at(nextPos);

                if (nextPiece.isEmpty())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::WHITE_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
                else if (nextPiece.isBlack())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(nextPos, nextPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::WHITE_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }

            }

            nextPos = from + glm::ivec2(-2, -1);
            if (nextPos.y > -1)
            {
                const auto& nextPiece = board.at(nextPos);

                if (nextPiece.isEmpty())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::WHITE_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
                else if (nextPiece.isBlack())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(nextPos, nextPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::WHITE_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
            }
        }

        return moves;
    }

    std::vector<Move> Calculator::getBlackKnightMoves(const Board& board, const glm::ivec2& from)
    {
        std::vector<Move> moves;
        moves.reserve(8);
        const auto& currentPiece = board.at(from);

        //forward moves
        if (from.y + 2 < 8)
        {
            auto nextPos = from + glm::ivec2(1, 2);
            if (nextPos.x < 8)
            {
                const auto& nextPiece = board.at(nextPos);

                if (nextPiece.isEmpty())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::BLACK_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
                else if (nextPiece.isWhite())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(nextPos, nextPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::BLACK_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }

            }

            nextPos = from + glm::ivec2(-1, 2);
            if (nextPos.x > -1)
            {
                const auto& nextPiece = board.at(nextPos);

                if (nextPiece.isEmpty())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::BLACK_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
                else if (nextPiece.isWhite())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(nextPos, nextPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::BLACK_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
            }
        }

        //backward moves
        if (from.y - 2 > -1)
        {
            auto nextPos = from + glm::ivec2(1, -2);
            if (nextPos.x < 8)
            {
                const auto& nextPiece = board.at(nextPos);

                if (nextPiece.isEmpty())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::BLACK_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
                else if (nextPiece.isWhite())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(nextPos, nextPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::BLACK_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }

            }

            nextPos = from + glm::ivec2(-1, -2);
            if (nextPos.x > -1)
            {
                const auto& nextPiece = board.at(nextPos);

                if (nextPiece.isEmpty())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::BLACK_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
                else if (nextPiece.isWhite())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(nextPos, nextPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::BLACK_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
            }
        }

        //right moves
        if (from.x + 2 < 8)
        {
            auto nextPos = from + glm::ivec2(2, 1);
            if (nextPos.y < 8)
            {
                const auto& nextPiece = board.at(nextPos);

                if (nextPiece.isEmpty())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::BLACK_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
                else if (nextPiece.isWhite())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(nextPos, nextPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::BLACK_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }

            }

            nextPos = from + glm::ivec2(2, -1);
            if (nextPos.y > -1)
            {
                const auto& nextPiece = board.at(nextPos);

                if (nextPiece.isEmpty())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::BLACK_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
                else if (nextPiece.isWhite())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(nextPos, nextPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::BLACK_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
            }
        }

        //left moves
        if (from.x - 2 > -1)
        {
            auto nextPos = from + glm::ivec2(-2, 1);
            if (nextPos.y < 8)
            {
                const auto& nextPiece = board.at(nextPos);

                if (nextPiece.isEmpty())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::BLACK_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
                else if (nextPiece.isWhite())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(nextPos, nextPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::BLACK_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }

            }

            nextPos = from + glm::ivec2(-2, -1);
            if (nextPos.y > -1)
            {
                const auto& nextPiece = board.at(nextPos);

                if (nextPiece.isEmpty())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::BLACK_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
                else if (nextPiece.isWhite())
                {
                    Move move;
                    move.from = from;
                    move.to = nextPos;
                    move.removedPieces.push_back(std::make_pair(from, currentPiece));
                    move.removedPieces.push_back(std::make_pair(nextPos, nextPiece));
                    move.addedPieces.push_back(std::make_pair(nextPos,
                        Piece(Piece::Type::BLACK_KNIGHT, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                    moves.push_back(move);
                }
            }
        }

        return moves;
    }


    std::vector<Move> Calculator::getWhiteBishopMoves(const Board& board, const glm::ivec2& from)
    {
        std::vector<Move> moves;
        moves.reserve(4);
        const auto& currentPiece = board.at(from);
        size_t offsetY = 1;

        int leftBottomLimit = std::min(from.x, from.y);
        int leftTopLimit = std::min(from.x, 7 - from.y);
        int rightBottomLimit = std::min(7 - from.x, from.y);
        int rightTopLimit = std::min(7 - from.x, 7 - from.y);

        checkDirection(leftBottomLimit, board, currentPiece,
            from, moves, [](const Piece& piece) { return !piece.isEmpty() && piece.isWhite(); },
            [](int i, const glm::ivec2& from) { return glm::ivec2(from.x - i, from.y - i); });

        checkDirection(leftTopLimit, board, currentPiece,
            from, moves, [](const Piece& piece) { return !piece.isEmpty() && piece.isWhite(); },
            [](int i, const glm::ivec2& from) { return glm::ivec2(from.x - i, from.y + i); });

        checkDirection(rightBottomLimit, board, currentPiece,
            from, moves, [](const Piece& piece) { return !piece.isEmpty() && piece.isWhite(); },
            [](int i, const glm::ivec2& from) { return glm::ivec2(from.x + i, from.y - i); });

        checkDirection(rightTopLimit, board, currentPiece,
            from, moves, [](const Piece& piece) { return !piece.isEmpty() && piece.isWhite(); },
            [](int i, const glm::ivec2& from) { return glm::ivec2(from.x + i, from.y + i); });

        return moves;
    }

    std::vector<Move> Calculator::getBlackBishopMoves(const Board& board, const glm::ivec2& from)
    {
        std::vector<Move> moves;
        moves.reserve(4);
        const auto& currentPiece = board.at(from);
        size_t offsetY = 1;

        int leftBottomLimit = std::min(from.x, from.y);
        int leftTopLimit = std::min(from.x, 7 - from.y);
        int rightBottomLimit = std::min(7 - from.x, from.y);
        int rightTopLimit = std::min(7 - from.x, 7 - from.y);

        checkDirection(leftBottomLimit, board, currentPiece,
            from, moves, [](const Piece& piece) { return piece.isBlack(); },
            [](int i, const glm::ivec2& from) { return glm::ivec2(from.x - i, from.y - i); });

        checkDirection(leftTopLimit, board, currentPiece,
            from, moves, [](const Piece& piece) { return piece.isBlack(); },
            [](int i, const glm::ivec2& from) { return glm::ivec2(from.x - i, from.y + i); });

        checkDirection(rightBottomLimit, board, currentPiece,
            from, moves, [](const Piece& piece) { return piece.isBlack(); },
            [](int i, const glm::ivec2& from) { return glm::ivec2(from.x + i, from.y - i); });

        checkDirection(rightTopLimit, board, currentPiece,
            from, moves, [](const Piece& piece) { return piece.isBlack(); },
            [](int i, const glm::ivec2& from) { return glm::ivec2(from.x + i, from.y + i); });

        return moves;
    }


    std::vector<Move> Calculator::getWhiteRookMoves(const Board& board, const glm::ivec2& from)
    {
        std::vector<Move> moves;
        moves.reserve(4);
        const auto& currentPiece = board.at(from);

        int upLimit = 7 - from.y;
        int downLimit = from.y;
        int leftLimit = from.x;
        int rightLimit = 7 - from.x;

        // Orthogonal movements using the same checkDirection function
        checkDirection(upLimit, board, currentPiece, from, moves,
            [](const Piece& piece) { return !piece.isEmpty() && piece.isWhite(); },
            [](int i, const glm::ivec2& from) { return glm::ivec2(from.x, from.y + i); });

        checkDirection(downLimit, board, currentPiece, from, moves,
            [](const Piece& piece) { return !piece.isEmpty() && piece.isWhite(); },
            [](int i, const glm::ivec2& from) { return glm::ivec2(from.x, from.y - i); });

        checkDirection(leftLimit, board, currentPiece, from, moves,
            [](const Piece& piece) { return !piece.isEmpty() && piece.isWhite(); },
            [](int i, const glm::ivec2& from) { return glm::ivec2(from.x - i, from.y); });

        checkDirection(rightLimit, board, currentPiece, from, moves,
            [](const Piece& piece) { return !piece.isEmpty() && piece.isWhite(); },
            [](int i, const glm::ivec2& from) { return glm::ivec2(from.x + i, from.y); });

        return moves;
    }

    std::vector<Move> Calculator::getBlackRookMoves(const Board& board, const glm::ivec2& from)
    {
        std::vector<Move> moves;
        moves.reserve(4);
        const auto& currentPiece = board.at(from);

        int upLimit = 7 - from.y;
        int downLimit = from.y;
        int leftLimit = from.x;
        int rightLimit = 7 - from.x;

        // Orthogonal movements using the same checkDirection function
        checkDirection(upLimit, board, currentPiece, from, moves,
            [](const Piece& piece) { return piece.isBlack(); },
            [](int i, const glm::ivec2& from) { return glm::ivec2(from.x, from.y + i); });

        checkDirection(downLimit, board, currentPiece, from, moves,
            [](const Piece& piece) { return piece.isBlack(); },
            [](int i, const glm::ivec2& from) { return glm::ivec2(from.x, from.y - i); });

        checkDirection(leftLimit, board, currentPiece, from, moves,
            [](const Piece& piece) { return piece.isBlack(); },
            [](int i, const glm::ivec2& from) { return glm::ivec2(from.x - i, from.y); });

        checkDirection(rightLimit, board, currentPiece, from, moves,
            [](const Piece& piece) { return piece.isBlack(); },
            [](int i, const glm::ivec2& from) { return glm::ivec2(from.x + i, from.y); });

        return moves;
    }


    std::vector<Move> Calculator::getWhiteQueenMoves(const Board& board, const glm::ivec2& from)
    {
        auto diagonalMoves = getWhiteBishopMoves(board, from);
        auto perpendicularMoves = getWhiteRookMoves(board, from);
        diagonalMoves.insert(diagonalMoves.end(), perpendicularMoves.begin(), perpendicularMoves.end());
        return diagonalMoves;
    }

    std::vector<Move> Calculator::getBlackQueenMoves(const Board& board, const glm::ivec2& from)
    {
        auto diagonalMoves = getBlackBishopMoves(board, from);
        auto perpendicularMoves = getBlackRookMoves(board, from);
        diagonalMoves.insert(diagonalMoves.end(), perpendicularMoves.begin(), perpendicularMoves.end());
        return diagonalMoves;
    }


    std::vector<Move> Calculator::getWhiteKingMoves(const Board& board, const glm::ivec2& from)
    {
        std::vector<Move> moves;
        moves.reserve(9);
        const auto& currentPiece = board.at(from);

        // Check diagonals for bishops and queens
        int bottomX = std::max(from.x - 1, 0);
        int topX = std::min(from.x + 1, 7);
        int bottomY = std::max(from.y - 1, 0);
        int topY = std::min(from.y + 1, 7);

        glm::ivec2 buf;

        for (int x = bottomX; x <= topX; ++x)
        {
            for (int y = bottomY; y <= topY; ++y)
            {
                buf = glm::ivec2(x, y);
                const auto& movedTo = board.at(buf);
                if (movedTo.isWhite() && !movedTo.isEmpty() || x == from.x && y == from.y)
                    continue;

                //no need for this check because it happens in get all possible moves
                //if (isCellUnderAttackWhite(board, glm::ivec2(x, y)))
                //    continue;

                Move move;
                move.from = from;
                move.to = buf;
                move.removedPieces.push_back(std::make_pair(from, currentPiece));
                move.addedPieces.push_back(std::make_pair(buf,
                    Piece(currentPiece.piece, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                if (!movedTo.isEmpty())
                    move.removedPieces.push_back(std::make_pair(buf, movedTo));
                moves.push_back(move);
            }
        }

        //check for castling

        if (currentPiece.moveCounter == 0)
        {
            if (board.at(0, 0).piece == Piece::Type::WHITE_ROOK &&
                board.at(0, 0).moveCounter == 0 &&
                board.at(1, 0).isEmpty() &&
                board.at(2, 0).isEmpty() &&
                board.at(3, 0).isEmpty())
            {
                const auto& rook = board.at(0, 0);
                Move move;
                move.from = from;
                move.to = glm::ivec2(2, 0);
                move.removedPieces.push_back(std::make_pair(from, currentPiece));
                move.removedPieces.push_back(std::make_pair(glm::ivec2(0, 0), rook));
                move.addedPieces.push_back(std::make_pair(glm::ivec2(2, 0),
                    Piece(currentPiece.piece, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                move.addedPieces.push_back(std::make_pair(glm::ivec2(3, 0),
                    Piece(rook.piece, rook.moveCounter + 1, rook.lastMoved + 1)));
                moves.push_back(move);

            }

            if (board.at(7, 0).piece == Piece::Type::WHITE_ROOK &&
                board.at(7, 0).moveCounter == 0 &&
                board.at(6, 0).isEmpty() &&
                board.at(5, 0).isEmpty())
            {
                const auto& rook = board.at(7, 0);
                Move move;
                move.from = from;
                move.to = glm::ivec2(6, 0);
                move.removedPieces.push_back(std::make_pair(from, currentPiece));
                move.removedPieces.push_back(std::make_pair(glm::ivec2(7, 0), rook));
                move.addedPieces.push_back(std::make_pair(glm::ivec2(6, 0),
                    Piece(currentPiece.piece, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                move.addedPieces.push_back(std::make_pair(glm::ivec2(5, 0),
                    Piece(rook.piece, rook.moveCounter + 1, rook.lastMoved + 1)));
                moves.push_back(move);
            }
        }

        return moves;
    }

    std::vector<Move> Calculator::getBlackKingMoves(const Board& board, const glm::ivec2& from)
    {
        std::vector<Move> moves;
        moves.reserve(9);
        const auto& currentPiece = board.at(from);


        int bottomX = std::max(from.x - 1, 0);
        int topX = std::min(from.x + 1, 7);
        int bottomY = std::max(from.y - 1, 0);
        int topY = std::min(from.y + 1, 7);

        glm::ivec2 buf;

        for (int x = bottomX; x <= topX; ++x)
        {
            for (int y = bottomY; y <= topY; ++y)
            {
                buf = glm::ivec2(x, y);
                const auto& movedTo = board.at(buf);
                if (movedTo.isBlack() || x == from.x && y == from.y)
                    continue;

                //no need for this check because it happens in get all possible moves
                //if (isCellUnderAttackBlack(board, glm::ivec2(x, y)))
                //    continue;

                Move move;
                move.from = from;
                move.to = buf;
                move.removedPieces.push_back(std::make_pair(from, currentPiece));
                move.addedPieces.push_back(std::make_pair(buf,
                    Piece(currentPiece.piece, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                if (!movedTo.isEmpty())
                    move.removedPieces.push_back(std::make_pair(buf, movedTo));
                moves.push_back(move);
            }
        }

        //check for castling

        if (currentPiece.moveCounter == 0)
        {
            if (board.at(0, 7).piece == Piece::Type::BLACK_ROOK &&
                board.at(0, 7).moveCounter == 0 &&
                board.at(1, 7).isEmpty() &&
                board.at(2, 7).isEmpty() &&
                board.at(3, 7).isEmpty())
            {
                const auto& rook = board.at(0, 7);
                Move move;
                move.from = from;
                move.to = glm::ivec2(2, 7);
                move.removedPieces.push_back(std::make_pair(from, currentPiece));
                move.removedPieces.push_back(std::make_pair(glm::ivec2(0, 7), rook));
                move.addedPieces.push_back(std::make_pair(glm::ivec2(2, 7),
                    Piece(currentPiece.piece, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                move.addedPieces.push_back(std::make_pair(glm::ivec2(3, 7),
                    Piece(rook.piece, rook.moveCounter + 1, rook.lastMoved + 1)));
                moves.push_back(move);

            }

            if (board.at(7, 7).piece == Piece::Type::BLACK_ROOK &&
                board.at(7, 7).moveCounter == 0 &&
                board.at(6, 7).isEmpty() &&
                board.at(5, 7).isEmpty())
            {
                const auto& rook = board.at(7, 7);
                Move move;
                move.from = from;
                move.to = glm::ivec2(6, 7);
                move.removedPieces.push_back(std::make_pair(from, currentPiece));
                move.removedPieces.push_back(std::make_pair(glm::ivec2(7, 7), rook));
                move.addedPieces.push_back(std::make_pair(glm::ivec2(6, 7),
                    Piece(currentPiece.piece, currentPiece.moveCounter + 1, board.getTotalMoveCount())));
                move.addedPieces.push_back(std::make_pair(glm::ivec2(5, 7),
                    Piece(rook.piece, rook.moveCounter + 1, rook.lastMoved + 1)));
                moves.push_back(move);
            }
        }

        return moves;
    }

    bool Calculator::isCellUnderAttackWhite(const Board& board, const glm::ivec2& pos)
    {
        int leftBottomLimit = std::min(pos.x, pos.y);
        int leftTopLimit = std::min(pos.x, 7 - pos.y);
        int rightBottomLimit = std::min(7 - pos.x, pos.y);
        int rightTopLimit = std::min(7 - pos.x, 7 - pos.y);

        if (checkDirectionForEnemy(board, pos, leftBottomLimit,
            [](const Piece& p) { return !p.isEmpty() && p.isWhite(); },
            [](int i, const glm::ivec2& pos) { return glm::ivec2(pos.x - i, pos.y - i); },
            [](const Piece& p, int i) {
                return p.piece == Piece::Type::BLACK_BISHOP || p.piece == Piece::Type::BLACK_QUEEN
                    || p.piece == Piece::Type::BLACK_KING && i == 1;
            })) return true;
        if (checkDirectionForEnemy(board, pos, leftTopLimit,
            [](const Piece& p) { return !p.isEmpty() && p.isWhite(); },
            [](int i, const glm::ivec2& pos) { return glm::ivec2(pos.x - i, pos.y + i); },
            [](const Piece& p, int i) {
                return p.piece == Piece::Type::BLACK_BISHOP || p.piece == Piece::Type::BLACK_QUEEN
                    || p.piece == Piece::Type::BLACK_KING && i == 1
                    || p.piece == Piece::Type::BLACK_PAWN && i == 1;
            })) return true;
        if (checkDirectionForEnemy(board, pos, rightBottomLimit,
            [](const Piece& p) { return !p.isEmpty() && p.isWhite(); },
            [](int i, const glm::ivec2& pos) { return glm::ivec2(pos.x + i, pos.y - i); },
            [](const Piece& p, int i) {
                return p.piece == Piece::Type::BLACK_BISHOP || p.piece == Piece::Type::BLACK_QUEEN
                    || p.piece == Piece::Type::BLACK_KING && i == 1;
            })) return true;
        if (checkDirectionForEnemy(board, pos, rightTopLimit,
            [](const Piece& p) { return !p.isEmpty() && p.isWhite(); },
            [](int i, const glm::ivec2& pos) { return glm::ivec2(pos.x + i, pos.y + i); },
            [](const Piece& p, int i) {
                return p.piece == Piece::Type::BLACK_BISHOP || p.piece == Piece::Type::BLACK_QUEEN
                    || p.piece == Piece::Type::BLACK_KING && i == 1
                    || p.piece == Piece::Type::BLACK_PAWN && i == 1;
            })) return true;


        // Check orthogonals for rooks and queens
        int upLimit = 7 - pos.y;
        int downLimit = pos.y;
        int leftLimit = pos.x;
        int rightLimit = 7 - pos.x;

        if (checkDirectionForEnemy(board, pos, upLimit,
            [](const Piece& p) { return !p.isEmpty() && p.isWhite(); },
            [](int i, const glm::ivec2& pos) { return glm::ivec2(pos.x, pos.y + i); },
            [](const Piece& p, int i) {
                return p.piece == Piece::Type::BLACK_ROOK || p.piece == Piece::Type::BLACK_QUEEN
                    || p.piece == Piece::Type::BLACK_KING && i == 1;
            })) return true;
        if (checkDirectionForEnemy(board, pos, downLimit,
            [](const Piece& p) { return !p.isEmpty() && p.isWhite(); },
            [](int i, const glm::ivec2& pos) { return glm::ivec2(pos.x, pos.y - i); },
            [](const Piece& p, int i) {
                return p.piece == Piece::Type::BLACK_ROOK || p.piece == Piece::Type::BLACK_QUEEN
                    || p.piece == Piece::Type::BLACK_KING && i == 1;
            })) return true;
        if (checkDirectionForEnemy(board, pos, leftLimit,
            [](const Piece& p) { return !p.isEmpty() && p.isWhite(); },
            [](int i, const glm::ivec2& pos) { return glm::ivec2(pos.x - i, pos.y); },
            [](const Piece& p, int i) {
                return p.piece == Piece::Type::BLACK_ROOK || p.piece == Piece::Type::BLACK_QUEEN
                    || p.piece == Piece::Type::BLACK_KING && i == 1;
            })) return true;
        if (checkDirectionForEnemy(board, pos, rightLimit,
            [](const Piece& p) { return !p.isEmpty() && p.isWhite(); },
            [](int i, const glm::ivec2& pos) { return glm::ivec2(pos.x + i, pos.y); },
            [](const Piece& p, int i) {
                return p.piece == Piece::Type::BLACK_ROOK || p.piece == Piece::Type::BLACK_QUEEN
                    || p.piece == Piece::Type::BLACK_KING && i == 1;
            })) return true;

        //check for knights
        if (pos.y + 2 < 8 &&
            (pos.x + 1 < 8 && board.at(pos.x + 1, pos.y + 2).piece == Piece::Type::BLACK_KNIGHT ||
                pos.x - 1 > -1 && board.at(pos.x - 1, pos.y + 2).piece == Piece::Type::BLACK_KNIGHT))
            return true;
        if (pos.y - 2 > -1 &&
            (pos.x + 1 < 8 && board.at(pos.x + 1, pos.y - 2).piece == Piece::Type::BLACK_KNIGHT ||
                pos.x - 1 > -1 && board.at(pos.x - 1, pos.y - 2).piece == Piece::Type::BLACK_KNIGHT))
            return true;
        if (pos.x + 2 < 8 &&
            (pos.y + 1 < 8 && board.at(pos.x + 2, pos.y + 1).piece == Piece::Type::BLACK_KNIGHT ||
                pos.y - 1 > -1 && board.at(pos.x + 2, pos.y - 1).piece == Piece::Type::BLACK_KNIGHT))
            return true;
        if (pos.x - 2 > -1 &&
            (pos.y + 1 < 8 && board.at(pos.x - 2, pos.y + 1).piece == Piece::Type::BLACK_KNIGHT ||
                pos.y - 1 > -1 && board.at(pos.x - 2, pos.y - 1).piece == Piece::Type::BLACK_KNIGHT))
            return true;

        return false;
    }

    bool Calculator::isCellUnderAttackBlack(const Board& board, const glm::ivec2& pos)
    {
        int leftBottomLimit = std::min(pos.x, pos.y);
        int leftTopLimit = std::min(pos.x, 7 - pos.y);
        int rightBottomLimit = std::min(7 - pos.x, pos.y);
        int rightTopLimit = std::min(7 - pos.x, 7 - pos.y);

        if (checkDirectionForEnemy(board, pos, leftBottomLimit,
            [](const Piece& p) { return p.isBlack(); },
            [](int i, const glm::ivec2& pos) { return glm::ivec2(pos.x - i, pos.y - i); },
            [](const Piece& p, int i) {
                return p.piece == Piece::Type::WHITE_BISHOP || p.piece == Piece::Type::WHITE_QUEEN
                    || p.piece == Piece::Type::WHITE_KING && i == 1
                    || p.piece == Piece::Type::WHITE_PAWN && i == 1;
            })) return true;
        if (checkDirectionForEnemy(board, pos, leftTopLimit,
            [](const Piece& p) { return p.isBlack(); },
            [](int i, const glm::ivec2& pos) { return glm::ivec2(pos.x - i, pos.y + i); },
            [](const Piece& p, int i) {
                return p.piece == Piece::Type::WHITE_BISHOP || p.piece == Piece::Type::WHITE_QUEEN
                    || p.piece == Piece::Type::WHITE_KING && i == 1;
            })) return true;
        if (checkDirectionForEnemy(board, pos, rightBottomLimit,
            [](const Piece& p) { return p.isBlack(); },
            [](int i, const glm::ivec2& pos) { return glm::ivec2(pos.x + i, pos.y - i); },
            [](const Piece& p, int i) {
                return p.piece == Piece::Type::WHITE_BISHOP || p.piece == Piece::Type::WHITE_QUEEN
                    || p.piece == Piece::Type::WHITE_KING && i == 1
                    || p.piece == Piece::Type::WHITE_PAWN && i == 1;
            })) return true;
        if (checkDirectionForEnemy(board, pos, rightTopLimit,
            [](const Piece& p) { return p.isBlack(); },
            [](int i, const glm::ivec2& pos) { return glm::ivec2(pos.x + i, pos.y + i); },
            [](const Piece& p, int i) {
                return p.piece == Piece::Type::WHITE_BISHOP || p.piece == Piece::Type::WHITE_QUEEN
                    || p.piece == Piece::Type::WHITE_KING && i == 1;
            })) return true;


        // Check orthogonals for rooks and queens
        int upLimit = 7 - pos.y;
        int downLimit = pos.y;
        int leftLimit = pos.x;
        int rightLimit = 7 - pos.x;

        if (checkDirectionForEnemy(board, pos, upLimit,
            [](const Piece& p) { return p.isBlack(); },
            [](int i, const glm::ivec2& pos) { return glm::ivec2(pos.x, pos.y + i); },
            [](const Piece& p, int i) {
                return p.piece == Piece::Type::WHITE_ROOK || p.piece == Piece::Type::WHITE_QUEEN
                    || p.piece == Piece::Type::WHITE_KING && i == 1;
            })) return true;
        if (checkDirectionForEnemy(board, pos, downLimit,
            [](const Piece& p) { return p.isBlack(); },
            [](int i, const glm::ivec2& pos) { return glm::ivec2(pos.x, pos.y - i); },
            [](const Piece& p, int i) {
                return p.piece == Piece::Type::WHITE_ROOK || p.piece == Piece::Type::WHITE_QUEEN
                    || p.piece == Piece::Type::WHITE_KING && i == 1;
            })) return true;
        if (checkDirectionForEnemy(board, pos, leftLimit,
            [](const Piece& p) { return p.isBlack(); },
            [](int i, const glm::ivec2& pos) { return glm::ivec2(pos.x - i, pos.y); },
            [](const Piece& p, int i) {
                return p.piece == Piece::Type::WHITE_ROOK || p.piece == Piece::Type::WHITE_QUEEN
                    || p.piece == Piece::Type::WHITE_KING && i == 1;
            })) return true;
        if (checkDirectionForEnemy(board, pos, rightLimit,
            [](const Piece& p) { return p.isBlack(); },
            [](int i, const glm::ivec2& pos) { return glm::ivec2(pos.x + i, pos.y); },
            [](const Piece& p, int i) {
                return p.piece == Piece::Type::WHITE_ROOK || p.piece == Piece::Type::WHITE_QUEEN
                    || p.piece == Piece::Type::WHITE_KING && i == 1;
            })) return true;

        //check for knights
        if (pos.y + 2 < 8 &&
            (pos.x + 1 < 8 && board.at(pos.x + 1, pos.y + 2).piece == Piece::Type::WHITE_KNIGHT ||
                pos.x - 1 > -1 && board.at(pos.x - 1, pos.y + 2).piece == Piece::Type::WHITE_KNIGHT))
            return true;
        if (pos.y - 2 > -1 &&
            (pos.x + 1 < 8 && board.at(pos.x + 1, pos.y - 2).piece == Piece::Type::WHITE_KNIGHT ||
                pos.x - 1 > -1 && board.at(pos.x - 1, pos.y - 2).piece == Piece::Type::WHITE_KNIGHT))
            return true;
        if (pos.x + 2 < 8 &&
            (pos.y + 1 < 8 && board.at(pos.x + 2, pos.y + 1).piece == Piece::Type::WHITE_KNIGHT ||
                pos.y - 1 > -1 && board.at(pos.x + 2, pos.y - 1).piece == Piece::Type::WHITE_KNIGHT))
            return true;
        if (pos.x - 2 > -1 &&
            (pos.y + 1 < 8 && board.at(pos.x - 2, pos.y + 1).piece == Piece::Type::WHITE_KNIGHT ||
                pos.y - 1 > -1 && board.at(pos.x - 2, pos.y - 1).piece == Piece::Type::WHITE_KNIGHT))
            return true;

        return false;
    }

}

