#include "Chess.h"

namespace Chess
{
    //what the actual fuck is this implementation
    //why does this work my god
    bool Calculator::isSquareUnderAttackWhite(const Board& board, int square)
    {
        uint64_t occupancy = board.getBitBoard().getAllPieces();
        uint64_t squareMask = 1ULL << square;

        // Pawn attacks
        uint64_t pawnAttacks = ((squareMask & ~FILE_H) >> 7)  | ((squareMask & ~FILE_A) >> 9);
        if (pawnAttacks & board.getBitBoard().blackPawns)
            return true;

        // Knight attacks
        if (KNIGHT_ATTACKS[square] & board.getBitBoard().blackKnights)
            return true;

        // King attacks (needed for king move validation)
        if (KING_ATTACKS[square] & board.getBitBoard().blackKing)
            return true;

        // Bishop/Queen attacks
        if (MagicBishops::getAttacks(square, occupancy) &
            (board.getBitBoard().blackBishops | board.getBitBoard().blackQueens))
            return true;

        // Rook/Queen attacks
        if (MagicRooks::getAttacks(square, occupancy) &
            (board.getBitBoard().blackRooks | board.getBitBoard().blackQueens))
            return true;
        return false;
    }

    bool Calculator::isSquareUnderAttackBlack(const Board& board, int square)
    {
        uint64_t occupancy = board.getBitBoard().getAllPieces();
        uint64_t squareMask = 1ULL << square;

        // Pawn attacks
        uint64_t pawnAttacks = ((squareMask & ~FILE_A) << 7) | ((squareMask & ~FILE_H) << 9);
        if (pawnAttacks & board.getBitBoard().whitePawns)
            return true;

        // Knight attacks
        if (KNIGHT_ATTACKS[square] & board.getBitBoard().whiteKnights)
            return true;

        // King attacks (needed for king move validation)
        if (KING_ATTACKS[square] & board.getBitBoard().whiteKing)
            return true;

        // Bishop/Queen attacks
        if (MagicBishops::getAttacks(square, occupancy) &
            (board.getBitBoard().whiteBishops | board.getBitBoard().whiteQueens))
            return true;

        // Rook/Queen attacks
        if (MagicRooks::getAttacks(square, occupancy) &
            (board.getBitBoard().whiteRooks | board.getBitBoard().whiteQueens))
            return true;
        return false;
    }

    void Calculator::getWhitePawnsMoves(const Board& board, std::vector<Board>& nextBoards)
	{
		uint64_t empty = ~board.getBitBoard().getAllPieces();
		uint64_t enemies = board.getBitBoard().getAllBlackPieces();
        uint64_t moves = 0;

        // Single push (shift up by 8)
        moves = (board.getBitBoard().whitePawns << 8) & empty;
        while (moves) {
            int destinationSquare = std::countr_zero(moves);
            uint64_t destinationMask = 1ULL << destinationSquare;
            // Create working copy of current board
            Board newBoard = board;
            newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move
            newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
            // Find the source square (the pawn that can move to this destination)
            uint64_t sourceSquare = destinationSquare - 8;
            uint64_t sourceMask = 1ULL << sourceSquare;

            newBoard.getBitBoard().whitePawns &= ~sourceMask;
            if (destinationMask & RANK_8)
                //will add support for other pieces later
                newBoard.getBitBoard().whiteQueens |= destinationMask;
            else newBoard.getBitBoard().whitePawns |= destinationMask;

            if (isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
            if (!isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                nextBoards.push_back(newBoard);

            moves &= moves - 1;
        }

        // Double push (only from rank 2)
        moves = (((board.getBitBoard().whitePawns & RANK_2) << 8) & empty) << 8 & empty;
        while (moves) {
            int destinationSquare = std::countr_zero(moves);
            uint64_t destinationMask = 1ULL << destinationSquare;
            // Create working copy of current board
            Board newBoard = board;
            newBoard.getEnPassantSquare() = destinationSquare - 8; //set en passant to the square that was jumped over
            newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
            // Find the source square (the pawn that can move to this destination)
            uint64_t sourceSquare = destinationSquare - 16;
            uint64_t sourceMask = 1ULL << sourceSquare;

            newBoard.getBitBoard().whitePawns &= ~sourceMask;
            newBoard.getBitBoard().whitePawns |= destinationMask;

            if (isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
            if (!isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                nextBoards.push_back(newBoard);

            moves &= moves - 1;
        }

        // Captures
        // Left captures (shift up and left, but not if on a-file)
        moves = ((board.getBitBoard().whitePawns & ~FILE_A) << 7) & enemies;
        while (moves) {
            int destinationSquare = std::countr_zero(moves);
            uint64_t destinationMask = 1ULL << destinationSquare;
            // Create working copy of current board
            Board newBoard = board;
            newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move
            newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
            // Find the source square (the pawn that can move to this destination)
            uint64_t sourceSquare = destinationSquare - 7;
            uint64_t sourceMask = 1ULL << sourceSquare;

            newBoard.getBitBoard().blackPawns &= ~destinationMask;
            newBoard.getBitBoard().blackKnights &= ~destinationMask;
            newBoard.getBitBoard().blackBishops &= ~destinationMask;
            newBoard.getBitBoard().blackRooks &= ~destinationMask;
            newBoard.getBitBoard().blackQueens &= ~destinationMask;

            newBoard.getBitBoard().whitePawns &= ~sourceMask;
            if (destinationMask & RANK_8)
                //will add support for other pieces later
                newBoard.getBitBoard().whiteQueens |= destinationMask;
            else newBoard.getBitBoard().whitePawns |= destinationMask;

            if (isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
            if (!isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                nextBoards.push_back(newBoard);

            moves &= moves - 1;
        }

        // Right captures (shift up and right, but not if on h-file)
        moves = ((board.getBitBoard().whitePawns & ~FILE_H) << 9) & enemies;
        while (moves) {
            int destinationSquare = std::countr_zero(moves);
            uint64_t destinationMask = 1ULL << destinationSquare;
            // Create working copy of current board
            Board newBoard = board;
            newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move
            newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
            // Find the source square (the pawn that can move to this destination)
            uint64_t sourceSquare = destinationSquare - 9;
            uint64_t sourceMask = 1ULL << sourceSquare;

            newBoard.getBitBoard().blackPawns &= ~destinationMask;
            newBoard.getBitBoard().blackKnights &= ~destinationMask;
            newBoard.getBitBoard().blackBishops &= ~destinationMask;
            newBoard.getBitBoard().blackRooks &= ~destinationMask;
            newBoard.getBitBoard().blackQueens &= ~destinationMask;

            newBoard.getBitBoard().whitePawns &= ~sourceMask;
            if (destinationMask & RANK_8)
                //will add support for other pieces later
                newBoard.getBitBoard().whiteQueens |= destinationMask;
            else newBoard.getBitBoard().whitePawns |= destinationMask;

            if (isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
            if (!isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                nextBoards.push_back(newBoard);

            moves &= moves - 1;
        }

        // En passant
        uint64_t enPassantMask = 1ULL << board.getEnPassantSquare();
        if ((enPassantMask & RANK_6) != 0)
        {
            moves = ((board.getBitBoard().whitePawns & ~FILE_A) << 7) & enPassantMask;
            while (moves) {
                int destinationSquare = std::countr_zero(moves);
                uint64_t destinationMask = 1ULL << destinationSquare;
                // Create working copy of current board
                Board newBoard = board;
                newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move
                newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
                // Find the source square (the pawn that can move to this destination)
                uint64_t sourceSquare = destinationSquare - 7;
                uint64_t sourceMask = 1ULL << sourceSquare;

                newBoard.getBitBoard().blackPawns &= ~(enPassantMask >> 8);
                newBoard.getBitBoard().whitePawns &= ~sourceMask;
                newBoard.getBitBoard().whitePawns |= destinationMask;

                if (isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                    newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
                if (!isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                    nextBoards.push_back(newBoard);

                moves &= moves - 1;
            }

            moves = ((board.getBitBoard().whitePawns & ~FILE_H) << 9) & enPassantMask;
            while (moves) {
                int destinationSquare = std::countr_zero(moves);
                uint64_t destinationMask = 1ULL << destinationSquare;
                // Create working copy of current board
                Board newBoard = board;
                newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move
                newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
                // Find the source square (the pawn that can move to this destination)
                uint64_t sourceSquare = destinationSquare - 9;
                uint64_t sourceMask = 1ULL << sourceSquare;

                newBoard.getBitBoard().blackPawns &= ~(enPassantMask >> 8);
                newBoard.getBitBoard().whitePawns &= ~sourceMask;
                newBoard.getBitBoard().whitePawns |= destinationMask;

                if (isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                    newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
                if (!isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                    nextBoards.push_back(newBoard);

                moves &= moves - 1;
            }
        }
	};

    void Calculator::getBlackPawnsMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        uint64_t empty = ~board.getBitBoard().getAllPieces();
        uint64_t enemies = board.getBitBoard().getAllWhitePieces();
        uint64_t moves = 0;

        // Single push (shift up by 8)
        moves = (board.getBitBoard().blackPawns >> 8) & empty;
        while (moves) {
            int destinationSquare = std::countr_zero(moves);
            uint64_t destinationMask = 1ULL << destinationSquare;
            // Create working copy of current board
            Board newBoard = board;
            newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move
            newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
            // Find the source square (the pawn that can move to this destination)
            uint64_t sourceSquare = destinationSquare + 8;
            uint64_t sourceMask = 1ULL << sourceSquare;

            newBoard.getBitBoard().blackPawns &= ~sourceMask;
            if (destinationMask & RANK_1)
                //will add support for other pieces later
                newBoard.getBitBoard().blackQueens |= destinationMask;
            else newBoard.getBitBoard().blackPawns |= destinationMask;

            if (isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
            if (!isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                nextBoards.push_back(newBoard);

            moves &= moves - 1;
        }

        // Double push (only from rank 7)
        moves = (((board.getBitBoard().blackPawns & RANK_7) >> 8) & empty) >> 8 & empty;
        while (moves) {
            int destinationSquare = std::countr_zero(moves);
            uint64_t destinationMask = 1ULL << destinationSquare;
            // Create working copy of current board
            Board newBoard = board;
            newBoard.getEnPassantSquare() = destinationSquare + 8; //set en passant to the square that was jumped over
            newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
            // Find the source square (the pawn that can move to this destination)
            uint64_t sourceSquare = destinationSquare + 16;
            uint64_t sourceMask = 1ULL << sourceSquare;

            newBoard.getBitBoard().blackPawns &= ~sourceMask;
            newBoard.getBitBoard().blackPawns |= destinationMask;

            if (isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
            if (!isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                nextBoards.push_back(newBoard);

            moves &= moves - 1;
        }

        // Captures
        // Left captures (shift down and right, but not if on h-file)
        moves = ((board.getBitBoard().blackPawns & ~FILE_H) >> 7) & enemies;
        while (moves) {
            int destinationSquare = std::countr_zero(moves);
            uint64_t destinationMask = 1ULL << destinationSquare;
            // Create working copy of current board
            Board newBoard = board;
            newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move
            newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
            // Find the source square (the pawn that can move to this destination)
            uint64_t sourceSquare = destinationSquare + 7;
            uint64_t sourceMask = 1ULL << sourceSquare;

            newBoard.getBitBoard().whitePawns &= ~destinationMask;
            newBoard.getBitBoard().whiteKnights &= ~destinationMask;
            newBoard.getBitBoard().whiteBishops &= ~destinationMask;
            newBoard.getBitBoard().whiteRooks &= ~destinationMask;
            newBoard.getBitBoard().whiteQueens &= ~destinationMask;

            newBoard.getBitBoard().blackPawns &= ~sourceMask;
            if (destinationMask & RANK_1)
                //will add support for other pieces later
                newBoard.getBitBoard().blackQueens |= destinationMask;
            else newBoard.getBitBoard().blackPawns |= destinationMask;

            if (isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
            if (!isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                nextBoards.push_back(newBoard);

            moves &= moves - 1;
        }

        // Right captures (shift down and left, but not if on a-file)
        moves = ((board.getBitBoard().blackPawns & ~FILE_A) >> 9) & enemies;
        while (moves) {
            int destinationSquare = std::countr_zero(moves);
            uint64_t destinationMask = 1ULL << destinationSquare;
            // Create working copy of current board
            Board newBoard = board;
            newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move
            newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
            // Find the source square (the pawn that can move to this destination)
            uint64_t sourceSquare = destinationSquare + 9;
            uint64_t sourceMask = 1ULL << sourceSquare;

            newBoard.getBitBoard().whitePawns &= ~destinationMask;
            newBoard.getBitBoard().whiteKnights &= ~destinationMask;
            newBoard.getBitBoard().whiteBishops &= ~destinationMask;
            newBoard.getBitBoard().whiteRooks &= ~destinationMask;
            newBoard.getBitBoard().whiteQueens &= ~destinationMask;

            newBoard.getBitBoard().blackPawns &= ~sourceMask;
            if (destinationMask & RANK_1)
                //will add support for other pieces later
                newBoard.getBitBoard().blackQueens |= destinationMask;
            else newBoard.getBitBoard().blackPawns |= destinationMask;

            if (isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
            if (!isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                nextBoards.push_back(newBoard);

            moves &= moves - 1;
        }

        // En passant
        uint64_t enPassantMask = 1ULL << board.getEnPassantSquare();
        if ((enPassantMask & RANK_3) != 0)
        {
            moves = ((board.getBitBoard().blackPawns & ~FILE_H) >> 7) & enPassantMask;
            while (moves) {
                int destinationSquare = std::countr_zero(moves);
                uint64_t destinationMask = 1ULL << destinationSquare;
                // Create working copy of current board
                Board newBoard = board;
                newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move
                newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
                // Find the source square (the pawn that can move to this destination)
                uint64_t sourceSquare = destinationSquare + 7;
                uint64_t sourceMask = 1ULL << sourceSquare;

                newBoard.getBitBoard().whitePawns &= ~(enPassantMask << 8);
                newBoard.getBitBoard().blackPawns &= ~sourceMask;
                newBoard.getBitBoard().blackPawns |= destinationMask;

                if (isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                    newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
                if (!isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                    nextBoards.push_back(newBoard);

                moves &= moves - 1;
            }

            moves = ((board.getBitBoard().blackPawns & ~FILE_A) >> 9) & enPassantMask;
            while (moves) {
                int destinationSquare = std::countr_zero(moves);
                uint64_t destinationMask = 1ULL << destinationSquare;
                // Create working copy of current board
                Board newBoard = board;
                newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move
                newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
                // Find the source square (the pawn that can move to this destination)
                uint64_t sourceSquare = destinationSquare + 9;
                uint64_t sourceMask = 1ULL << sourceSquare;

                newBoard.getBitBoard().whitePawns &= ~(enPassantMask << 8);
                newBoard.getBitBoard().blackPawns &= ~sourceMask;
                newBoard.getBitBoard().blackPawns |= destinationMask;

                if (isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                    newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
                if (!isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                    nextBoards.push_back(newBoard);

                moves &= moves - 1;
            }
        }
    };

    void Calculator::getWhiteKnightsMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        uint64_t friendly = board.getBitBoard().getAllWhitePieces();
        uint64_t knights = board.getBitBoard().whiteKnights;

        while (knights) {
            int sourceSquare = std::countr_zero(knights);
            uint64_t sourceMask = 1ULL << sourceSquare;

            uint64_t destinationsSquaresMask = KNIGHT_ATTACKS[sourceSquare] & ~friendly;
            while (destinationsSquaresMask) {
                // Create working copy of current board
                int destinationSquare = std::countr_zero(destinationsSquaresMask);
                uint64_t destinationMask = 1ULL << destinationSquare;

                Board newBoard = board;
                newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move
                newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
                newBoard.getBitBoard().whiteKnights &= ~sourceMask;
                newBoard.getBitBoard().whiteKnights |= destinationMask;

                newBoard.getBitBoard().blackPawns &= ~destinationMask;
                newBoard.getBitBoard().blackKnights &= ~destinationMask;
                newBoard.getBitBoard().blackBishops &= ~destinationMask;
                newBoard.getBitBoard().blackRooks &= ~destinationMask;
                newBoard.getBitBoard().blackQueens &= ~destinationMask;

                if (isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                    newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
                if (!isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                    nextBoards.push_back(newBoard);

                destinationsSquaresMask &= destinationsSquaresMask - 1;
            }
            knights &= knights - 1;
        }
    };

    void Calculator::getBlackKnightsMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        uint64_t friendly = board.getBitBoard().getAllBlackPieces();
        uint64_t knights = board.getBitBoard().blackKnights;

        while (knights) {
            int sourceSquare = std::countr_zero(knights);
            uint64_t sourceMask = 1ULL << sourceSquare;

            uint64_t destinationsSquaresMask = KNIGHT_ATTACKS[sourceSquare] & ~friendly;
            while (destinationsSquaresMask) {
                // Create working copy of current board
                int destinationSquare = std::countr_zero(destinationsSquaresMask);
                uint64_t destinationMask = 1ULL << destinationSquare;

                Board newBoard = board;
                newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move
                newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
                newBoard.getBitBoard().blackKnights &= ~sourceMask;
                newBoard.getBitBoard().blackKnights |= destinationMask;

                newBoard.getBitBoard().whitePawns &= ~destinationMask;
                newBoard.getBitBoard().whiteKnights &= ~destinationMask;
                newBoard.getBitBoard().whiteBishops &= ~destinationMask;
                newBoard.getBitBoard().whiteRooks &= ~destinationMask;
                newBoard.getBitBoard().whiteQueens &= ~destinationMask;

                if (isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                    newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
                if (!isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                    nextBoards.push_back(newBoard);

                destinationsSquaresMask &= destinationsSquaresMask - 1;
            }
            knights &= knights - 1;
        }
    }

    void Calculator::getWhiteBishopsMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        uint64_t friendly = board.getBitBoard().getAllWhitePieces();
        uint64_t bishops = board.getBitBoard().whiteBishops;

        while (bishops) {
            int sourceSquare = std::countr_zero(bishops);
            uint64_t sourceMask = 1ULL << sourceSquare;

            uint64_t destinationsSquaresMask = MagicBishops::getAttacks(sourceSquare,
                board.getBitBoard().getAllPieces()) & ~friendly;

            while (destinationsSquaresMask) {
                // Create working copy of current board
                int destinationSquare = std::countr_zero(destinationsSquaresMask);
                uint64_t destinationMask = 1ULL << destinationSquare;

                Board newBoard = board;
                newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move
                newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
                newBoard.getBitBoard().whiteBishops &= ~sourceMask;
                newBoard.getBitBoard().whiteBishops |= destinationMask;

                newBoard.getBitBoard().blackPawns &= ~destinationMask;
                newBoard.getBitBoard().blackKnights &= ~destinationMask;
                newBoard.getBitBoard().blackBishops &= ~destinationMask;
                newBoard.getBitBoard().blackRooks &= ~destinationMask;
                newBoard.getBitBoard().blackQueens &= ~destinationMask;

                if (isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                    newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
                if (!isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                    nextBoards.push_back(newBoard);

                destinationsSquaresMask &= destinationsSquaresMask - 1;
            }
            bishops &= bishops - 1;
        }
    }

    void Calculator::getBlackBishopsMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        uint64_t friendly = board.getBitBoard().getAllBlackPieces();
        uint64_t bishops = board.getBitBoard().blackBishops;

        while (bishops) {
            int sourceSquare = std::countr_zero(bishops);
            uint64_t sourceMask = 1ULL << sourceSquare;

            uint64_t destinationsSquaresMask = MagicBishops::getAttacks(sourceSquare,
                board.getBitBoard().getAllPieces()) & ~friendly;

            while (destinationsSquaresMask) {
                // Create working copy of current board
                int destinationSquare = std::countr_zero(destinationsSquaresMask);
                uint64_t destinationMask = 1ULL << destinationSquare;

                Board newBoard = board;
                newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move
                newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
                newBoard.getBitBoard().blackBishops &= ~sourceMask;
                newBoard.getBitBoard().blackBishops |= destinationMask;

                newBoard.getBitBoard().whitePawns &= ~destinationMask;
                newBoard.getBitBoard().whiteKnights &= ~destinationMask;
                newBoard.getBitBoard().whiteBishops &= ~destinationMask;
                newBoard.getBitBoard().whiteRooks &= ~destinationMask;
                newBoard.getBitBoard().whiteQueens &= ~destinationMask;

                if (isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                    newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
                if (!isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                    nextBoards.push_back(newBoard);

                destinationsSquaresMask &= destinationsSquaresMask - 1;
            }
            bishops &= bishops - 1;
        }
    }

    void Calculator::getWhiteRooksMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        uint64_t friendly = board.getBitBoard().getAllWhitePieces();
        uint64_t rooks = board.getBitBoard().whiteRooks;

        while (rooks) {
            int sourceSquare = std::countr_zero(rooks);
            uint64_t sourceMask = 1ULL << sourceSquare;

            uint64_t destinationsSquaresMask = MagicRooks::getAttacks(sourceSquare,
                board.getBitBoard().getAllPieces()) & ~friendly;

            while (destinationsSquaresMask) {
                // Create working copy of current board
                int destinationSquare = std::countr_zero(destinationsSquaresMask);
                uint64_t destinationMask = 1ULL << destinationSquare;

                Board newBoard = board;
                newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::WHITE_HAS_CASTLING_KINGSIDE_RIGHTS);
                newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::WHITE_HAS_CASTLING_QUEENSIDE_RIGHTS);
                newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
                newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move

                newBoard.getBitBoard().whiteRooks &= ~sourceMask;
                newBoard.getBitBoard().whiteRooks |= destinationMask;

                newBoard.getBitBoard().blackPawns &= ~destinationMask;
                newBoard.getBitBoard().blackKnights &= ~destinationMask;
                newBoard.getBitBoard().blackBishops &= ~destinationMask;
                newBoard.getBitBoard().blackRooks &= ~destinationMask;
                newBoard.getBitBoard().blackQueens &= ~destinationMask;

                if (isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                    newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
                if (!isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                    nextBoards.push_back(newBoard);

                destinationsSquaresMask &= destinationsSquaresMask - 1;
            }
            rooks &= rooks - 1;
        }
    }

    void Calculator::getBlackRooksMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        uint64_t friendly = board.getBitBoard().getAllBlackPieces();
        uint64_t rooks = board.getBitBoard().blackRooks;

        while (rooks) {
            int sourceSquare = std::countr_zero(rooks);
            uint64_t sourceMask = 1ULL << sourceSquare;

            uint64_t destinationsSquaresMask = MagicRooks::getAttacks(sourceSquare,
                board.getBitBoard().getAllPieces()) & ~friendly;

            while (destinationsSquaresMask) {
                // Create working copy of current board
                int destinationSquare = std::countr_zero(destinationsSquaresMask);
                uint64_t destinationMask = 1ULL << destinationSquare;

                Board newBoard = board;
                newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::BLACK_HAS_CASTLING_KINGSIDE_RIGHTS);
                newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::BLACK_HAS_CASTLING_QUEENSIDE_RIGHTS);
                newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
                newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move

                newBoard.getBitBoard().blackRooks &= ~sourceMask;
                newBoard.getBitBoard().blackRooks |= destinationMask;

                newBoard.getBitBoard().whitePawns &= ~destinationMask;
                newBoard.getBitBoard().whiteKnights &= ~destinationMask;
                newBoard.getBitBoard().whiteBishops &= ~destinationMask;
                newBoard.getBitBoard().whiteRooks &= ~destinationMask;
                newBoard.getBitBoard().whiteQueens &= ~destinationMask;

                if (isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                    newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
                if (!isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                    nextBoards.push_back(newBoard);

                destinationsSquaresMask &= destinationsSquaresMask - 1;
            }
            rooks &= rooks - 1;
        }
    }

    void Calculator::getWhiteQueensMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        uint64_t allPieces = board.getBitBoard().getAllPieces();
        uint64_t friendly = board.getBitBoard().getAllWhitePieces();
        uint64_t queens = board.getBitBoard().whiteQueens;

        while (queens) {
            int sourceSquare = std::countr_zero(queens);
            uint64_t sourceMask = 1ULL << sourceSquare;

            uint64_t destinationsSquaresMask = MagicBishops::getAttacks(sourceSquare,
                allPieces) & ~friendly;
            destinationsSquaresMask |= MagicRooks::getAttacks(sourceSquare,
                allPieces) & ~friendly;

            while (destinationsSquaresMask) {
                // Create working copy of current board
                int destinationSquare = std::countr_zero(destinationsSquaresMask);
                uint64_t destinationMask = 1ULL << destinationSquare;

                Board newBoard = board;
                newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move
                newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
                newBoard.getBitBoard().whiteQueens &= ~sourceMask;
                newBoard.getBitBoard().whiteQueens |= destinationMask;

                newBoard.getBitBoard().blackPawns &= ~destinationMask;
                newBoard.getBitBoard().blackKnights &= ~destinationMask;
                newBoard.getBitBoard().blackBishops &= ~destinationMask;
                newBoard.getBitBoard().blackRooks &= ~destinationMask;
                newBoard.getBitBoard().blackQueens &= ~destinationMask;

                if (isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                    newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
                if (!isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                    nextBoards.push_back(newBoard);

                destinationsSquaresMask &= destinationsSquaresMask - 1;
            }
            queens &= queens - 1;
        }
    }

    void Calculator::getBlackQueensMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        uint64_t allPieces = board.getBitBoard().getAllPieces();
        uint64_t friendly = board.getBitBoard().getAllBlackPieces();
        uint64_t queens = board.getBitBoard().blackQueens;

        while (queens) {
            int sourceSquare = std::countr_zero(queens);
            uint64_t sourceMask = 1ULL << sourceSquare;

            uint64_t destinationsSquaresMask = MagicBishops::getAttacks(sourceSquare,
                allPieces) & ~friendly;
            destinationsSquaresMask |= MagicRooks::getAttacks(sourceSquare,
                allPieces) & ~friendly;

            while (destinationsSquaresMask) {
                // Create working copy of current board
                int destinationSquare = std::countr_zero(destinationsSquaresMask);
                uint64_t destinationMask = 1ULL << destinationSquare;

                Board newBoard = board;
                newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move
                newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
                newBoard.getBitBoard().blackQueens &= ~sourceMask;
                newBoard.getBitBoard().blackQueens |= destinationMask;

                newBoard.getBitBoard().whitePawns &= ~destinationMask;
                newBoard.getBitBoard().whiteKnights &= ~destinationMask;
                newBoard.getBitBoard().whiteBishops &= ~destinationMask;
                newBoard.getBitBoard().whiteRooks &= ~destinationMask;
                newBoard.getBitBoard().whiteQueens &= ~destinationMask;
                if(isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                    newBoard.getFlags() |= static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
                if (!isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                    nextBoards.push_back(newBoard);

                destinationsSquaresMask &= destinationsSquaresMask - 1;
            }
            queens &= queens - 1;
        }
    }

    void Calculator::getWhiteKingMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        uint64_t empty = ~board.getBitBoard().getAllPieces();
        uint64_t friendly = board.getBitBoard().getAllWhitePieces();
        uint64_t king = board.getBitBoard().whiteKing;

        int sourceSquare = std::countr_zero(king);
        uint64_t sourceMask = 1ULL << sourceSquare;

        uint64_t destinationsSquaresMask = KING_ATTACKS[sourceSquare] & ~friendly;

        while (destinationsSquaresMask) {
            // Create working copy of current board
            int destinationSquare = std::countr_zero(destinationsSquaresMask);
            uint64_t destinationMask = 1ULL << destinationSquare;

            Board newBoard = board;
            newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::WHITE_HAS_CASTLING_KINGSIDE_RIGHTS);
            newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::WHITE_HAS_CASTLING_QUEENSIDE_RIGHTS);
            newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::WHITE_CHECKED);
            newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move

            newBoard.getBitBoard().whiteKing &= ~sourceMask;
            newBoard.getBitBoard().whiteKing |= destinationMask;

            newBoard.getBitBoard().blackPawns &= ~destinationMask;
            newBoard.getBitBoard().blackKnights &= ~destinationMask;
            newBoard.getBitBoard().blackBishops &= ~destinationMask;
            newBoard.getBitBoard().blackRooks &= ~destinationMask;
            newBoard.getBitBoard().blackQueens &= ~destinationMask;

            if (!isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                nextBoards.push_back(newBoard);

            destinationsSquaresMask &= destinationsSquaresMask - 1;
        }

        if ((board.getFlags() & static_cast<uint8_t>(Board::Flags::WHITE_HAS_CASTLING_KINGSIDE_RIGHTS)) &&
            (empty & WHITE_KINGSIDE_CASTLING_PATH) == WHITE_KINGSIDE_CASTLING_PATH)
        {
            Board newBoard = board;
            newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::WHITE_HAS_CASTLING_KINGSIDE_RIGHTS);
            newBoard.getEnPassantSquare() = 0;

            newBoard.getBitBoard().whiteKing &= ~sourceMask;
            newBoard.getBitBoard().whiteKing |= WHITE_KING_START << 2;
            newBoard.getBitBoard().whiteRooks &= ~WHITE_ROOK_KINGSIDE_START;
            newBoard.getBitBoard().whiteRooks |= WHITE_ROOK_KINGSIDE_START >> 2;

            if (!isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                nextBoards.push_back(newBoard);
        }

        if ((board.getFlags() & static_cast<uint8_t>(Board::Flags::WHITE_HAS_CASTLING_QUEENSIDE_RIGHTS)) &&
            (empty & WHITE_QUEENSIDE_CASTLING_PATH) == WHITE_QUEENSIDE_CASTLING_PATH)
        {
            Board newBoard = board;
            newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::WHITE_HAS_CASTLING_QUEENSIDE_RIGHTS);
            newBoard.getEnPassantSquare() = 0;

            newBoard.getBitBoard().whiteKing &= ~sourceMask;
            newBoard.getBitBoard().whiteKing |= WHITE_KING_START >> 2;
            newBoard.getBitBoard().whiteRooks &= ~WHITE_ROOK_QUEENSIDE_START;
            newBoard.getBitBoard().whiteRooks |= WHITE_ROOK_QUEENSIDE_START << 3;

            if (!isSquareUnderAttackWhite(newBoard, std::countr_zero(newBoard.getBitBoard().whiteKing)))
                nextBoards.push_back(newBoard);
        }
    }

    void Calculator::getBlackKingMoves(const Board& board, std::vector<Board>& nextBoards)
    {
        uint64_t empty = ~board.getBitBoard().getAllPieces();
        uint64_t friendly = board.getBitBoard().getAllBlackPieces();
        uint64_t king = board.getBitBoard().blackKing;

        int sourceSquare = std::countr_zero(king);
        uint64_t sourceMask = 1ULL << sourceSquare;

        uint64_t destinationsSquaresMask = KING_ATTACKS[sourceSquare] & ~friendly;

        while (destinationsSquaresMask) {
            // Create working copy of current board
            int destinationSquare = std::countr_zero(destinationsSquaresMask);
            uint64_t destinationMask = 1ULL << destinationSquare;

            Board newBoard = board;
            newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::BLACK_HAS_CASTLING_KINGSIDE_RIGHTS);
            newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::BLACK_HAS_CASTLING_QUEENSIDE_RIGHTS);
            newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::BLACK_CHECKED);
            newBoard.getEnPassantSquare() = 0; //set en passant to 0 for next move

            newBoard.getBitBoard().blackKing &= ~sourceMask;
            newBoard.getBitBoard().blackKing |= destinationMask;

            newBoard.getBitBoard().whitePawns &= ~destinationMask;
            newBoard.getBitBoard().whiteKnights &= ~destinationMask;
            newBoard.getBitBoard().whiteBishops &= ~destinationMask;
            newBoard.getBitBoard().whiteRooks &= ~destinationMask;
            newBoard.getBitBoard().whiteQueens &= ~destinationMask;

            if (!isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                nextBoards.push_back(newBoard);

            destinationsSquaresMask &= destinationsSquaresMask - 1;
        }

        if ((board.getFlags() & static_cast<uint8_t>(Board::Flags::BLACK_HAS_CASTLING_KINGSIDE_RIGHTS)) &&
            (empty & BLACK_KINGSIDE_CASTLING_PATH) == BLACK_KINGSIDE_CASTLING_PATH)
        {
            Board newBoard = board;
            newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::BLACK_HAS_CASTLING_KINGSIDE_RIGHTS);
            newBoard.getEnPassantSquare() = 0;

            newBoard.getBitBoard().blackKing &= ~sourceMask;
            newBoard.getBitBoard().blackKing |= BLACK_KING_START << 2;
            newBoard.getBitBoard().blackRooks &= ~BLACK_ROOK_KINGSIDE_START;
            newBoard.getBitBoard().blackRooks |= BLACK_ROOK_KINGSIDE_START >> 2;

            if (!isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                nextBoards.push_back(newBoard);
        }

        if ((board.getFlags() & static_cast<uint8_t>(Board::Flags::BLACK_HAS_CASTLING_QUEENSIDE_RIGHTS)) &&
            (empty & BLACK_QUEENSIDE_CASTLING_PATH) == BLACK_QUEENSIDE_CASTLING_PATH)
        {
            Board newBoard = board;
            newBoard.getFlags() &= ~static_cast<uint8_t>(Board::Flags::BLACK_HAS_CASTLING_QUEENSIDE_RIGHTS);
            newBoard.getEnPassantSquare() = 0;

            newBoard.getBitBoard().blackKing &= ~sourceMask;
            newBoard.getBitBoard().blackKing |= BLACK_KING_START >> 2;
            newBoard.getBitBoard().blackRooks &= ~BLACK_ROOK_QUEENSIDE_START;
            newBoard.getBitBoard().blackRooks |= BLACK_ROOK_QUEENSIDE_START << 3;

            if (!isSquareUnderAttackBlack(newBoard, std::countr_zero(newBoard.getBitBoard().blackKing)))
                nextBoards.push_back(newBoard);
        }
    }

};