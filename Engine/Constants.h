#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#ifdef _MSC_VER
#include <intrin.h>
#define popcnt64 __popcnt64
#else
#define popcnt64 __builtin_popcountll
#endif

#include <array>
#include <bit>
#include <iostream>
#include <span>

#include "MultiThreading/ThreadPool.h"

namespace Chess
{
    // Board orientation constants
    static inline const glm::ivec2 BOTTOM_LEFT = glm::ivec2(0, 0);
    static inline const glm::ivec2 TOP_RIGHT = glm::ivec2(7, 7);
    static inline const glm::ivec2 LEFT = glm::ivec2(-1, 0);
    static inline const glm::ivec2 RIGHT = glm::ivec2(1, 0);
    static inline const glm::ivec2 UP = glm::ivec2(0, 1);
    static inline const glm::ivec2 DOWN = glm::ivec2(0, -1);

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
    static constexpr std::array<std::array<int, 2>, 8> KNIGHT_MOVES = { {
        {1, 2}, {-1, 2},    // Forward
        {1, -2}, {-1, -2},  // Backward
        {2, 1}, {2, -1},    // Right
        {-2, 1}, {-2, -1}   // Left
    } };

    static inline const size_t MAXIMUM_CONSERVATIVE_MOVE_AMOUNT = 50; //may be higher but its unlikely

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
        5,  10, 10,-20,-20, 10, 10, 5,
        5,  -5,-10,  0,  0,-10, -5, 5,
        0,   0,  0, 20, 20,  0,  0, 0,
        5,   5, 10, 25, 25, 10,  5, 5,
        10, 10, 20, 30, 30, 20, 10, 10,
        50, 50, 50, 50, 50, 50, 50, 50,
        0,  0,  0,  0,  0,  0,  0,  0
    };

    static inline const std::array<int, 64> whiteKnightTable = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    };

    static inline const std::array<int, 64> whiteBishopTable = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    };

    static inline const std::array<int, 64> whiteRookTable = {
        0,  0,  0,  5,  5,  0,  0,  0,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        5, 10, 10, 10, 10, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0
    };

    static inline const std::array<int, 64> whiteQueenTable = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  5,  0,-10,
        -10,  0,  5,  5,  5,  5,  5,-10,
        -5,  0,  5,  5,  5,  5,  0,  0,
        -5,  0,  5,  5,  5,  5,  0, -5,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    };

    static inline const std::array<int, 64> whiteKingTable = {
        20, 30, 10,  0,  0, 10, 30, 20,
        20, 20,  0,  0,  0,  0, 20, 20,
        -10,-20,-20,-20,-20,-20,-20,-10,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30
    };

    static inline const std::array<int, 64> blackPawnTable = {
            0,  0,  0,  0,  0,  0,  0,  0,
            -50,-50,-50,-50,-50,-50,-50,-50,
            -10,-10,-20,-30,-30,-20,-10,-10,
            -5, -5,-10,-25,-25,-10, -5, -5,
            0,  0,  0,-20,-20,  0,  0,  0,
            -5,  5, 10,  0,  0, 10,  5, -5,
            -5,-10,-10, 20, 20,-10,-10, -5,
            0,  0,  0,  0,  0,  0,  0,  0
    };

    static inline const std::array<int, 64> blackKnightTable = {
        50, 40, 30, 30, 30, 30, 40, 50,
        40, 20,  0,  0,  0,  0, 20, 40,
        30,  0,-10,-15,-15,-10,  0, 30,
        30, -5,-15,-20,-20,-15, -5, 30,
        30,  0,-15,-20,-20,-15,  0, 30,
        30, -5,-10,-15,-15,-10, -5, 30,
        40, 20,  0, -5, -5,  0, 20, 40,
        50, 40, 30, 30, 30, 30, 40, 50
    };

    static inline const std::array<int, 64> blackBishopTable = {
        20, 10, 10, 10, 10, 10, 10, 20,
        10,  0,  0,  0,  0,  0,  0, 10,
        10,  0, -5,-10,-10, -5,  0, 10,
        10, -5, -5,-10,-10, -5, -5, 10,
        10,  0,-10,-10,-10,-10,  0, 10,
        10,-10,-10,-10,-10,-10,-10, 10,
        10, -5,  0,  0,  0,  0, -5, 10,
        20, 10, 10, 10, 10, 10, 10, 20
    };

    static inline const std::array<int, 64> blackRookTable = {
        0,  0,  0,  0,  0,  0,  0,  0,
        -5,-10,-10,-10,-10,-10,-10, -5,
        5,  0,  0,  0,  0,  0,  0,  5,
        5,  0,  0,  0,  0,  0,  0,  5,
        5,  0,  0,  0,  0,  0,  0,  5,
        5,  0,  0,  0,  0,  0,  0,  5,
        5,  0,  0,  0,  0,  0,  0,  5,
        0,  0,  0, -5, -5,  0,  0,  0
    };

    static inline const std::array<int, 64> blackQueenTable = {
        20, 10, 10,  5,  5, 10, 10, 20,
        10,  0,  0,  0,  0,  0,  0, 10,
        10,  0, -5, -5, -5, -5,  0, 10,
        5,  0, -5, -5, -5, -5,  0,  5,
        0,  0, -5, -5, -5, -5,  0,  5,
        10,  0, -5, -5, -5, -5, -5, 10,
        10,  0,  0,  0,  0, -5,  0, 10,
        20, 10, 10,  5,  5, 10, 10, 20
    };

    static inline const std::array<int, 64> blackKingTable = {
        30, 40, 40, 50, 50, 40, 40, 30,
        30, 40, 40, 50, 50, 40, 40, 30,
        30, 40, 40, 50, 50, 40, 40, 30,
        30, 40, 40, 50, 50, 40, 40, 30,
        20, 30, 30, 40, 40, 30, 30, 20,
        10, 20, 20, 20, 20, 20, 20, 10,
        -20,-20,  0,  0,  0,  0,-20,-20,
        -20,-30,-10,  0,  0,-10,-30,-20
    };


    // Pre-calculated lookup tables
    constexpr std::array<uint64_t, 64> KNIGHT_ATTACKS = []()->std::array<uint64_t, 64> {
        std::array<uint64_t, 64> attacks = { 0 };
        for (int i = 0; i < 64; i++)
        {
            int x = i % 8;
            int y = i / 8;
            for (int j = 0; j < KNIGHT_MOVES.size(); j++)
            {
                int newX = x + KNIGHT_MOVES[j][0];
                int newY = y + KNIGHT_MOVES[j][1];
                uint32_t square = newY * 8 + newX;

                if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
                    attacks[i] |= 1ULL << square;
                }
            }
        }
        return attacks;
        }();

    constexpr std::array<uint64_t, 64> BISHOP_ATTACKS = []()->std::array<uint64_t, 64> {
        std::array<uint64_t, 64> attacks = { 0 };
        for (int i = 0; i < 64; i++)
        {
            int x = i % 8;
            int y = i / 8;
            int offset = -x;

            for (int j = 0; j < 8; j++, offset++)
            {
                if (offset == 0)
                    continue;
                int newX = x + offset;
                int newY = y + offset;
                uint32_t square = newY * 8 + newX;

                if (newY >= 0 && newY < 8) {
                    attacks[i] |= 1ULL << square;
                }

                newY = y - offset;
                square = newY * 8 + newX;

                if (newY >= 0 && newY < 8) {
                    attacks[i] |= 1ULL << square;
                }
            }
        }
        return attacks;
        }();

    constexpr std::array<uint64_t, 64> ROOK_ATTACKS = []()->std::array<uint64_t, 64> {
        std::array<uint64_t, 64> attacks = { 0 };
        for (int i = 0; i < 64; i++)
        {
            int x = i % 8;
            int y = i / 8;

            for (int j = 0; j < 8; j++) {
                if (j != x) attacks[i] |= 1ULL << (y * 8 + j);
                if (j != y) attacks[i] |= 1ULL << (j * 8 + x);
            }
        }
        return attacks;
        }();

    constexpr std::array<uint64_t, 64> QUEEN_ATTACKS = []()->std::array<uint64_t, 64> {
        std::array<uint64_t, 64> attacks = { 0 };
        for (int i = 0; i < 64; i++)
            attacks[i] = ROOK_ATTACKS[i] | BISHOP_ATTACKS[i];

        return attacks;
        }();

    constexpr std::array<uint64_t, 64> KING_ATTACKS = []()->std::array<uint64_t, 64> {
        std::array<uint64_t, 64> attacks = { 0 };
        for (int i = 0; i < 64; i++)
        {
            int x = i % 8;
            int y = i / 8;

            int minX = std::max(0, x - 1);
            int maxX = std::min(7, x + 1);
            int minY = std::max(0, y - 1);
            int maxY = std::min(7, y + 1);

            for (int j = minX; j <= maxX; j++) {
                for (int k = minY; k <= maxY; k++) {
                    attacks[i] |= 1ULL << (k * 8 + j);
                }
            }
        }
        return attacks;
        }();

    static inline const uint64_t RANK_1 = 0x00000000000000FFULL; //white start here, its down, x = 0
    static inline const uint64_t RANK_2 = 0x000000000000FF00ULL;
    static inline const uint64_t RANK_3 = 0x0000000000FF0000ULL;
    static inline const uint64_t RANK_4 = 0x00000000FF000000ULL;
    static inline const uint64_t RANK_5 = 0x000000FF00000000ULL;
    static inline const uint64_t RANK_6 = 0x0000FF0000000000ULL;
    static inline const uint64_t RANK_7 = 0x00FF000000000000ULL;
    static inline const uint64_t RANK_8 = 0xFF00000000000000ULL; //top rank

    static inline const uint64_t FILE_A = 0x0101010101010101ULL; // file A (rightmost bit in each byte)
    static inline const uint64_t FILE_B = 0x0202020202020202ULL; // file B (second bit from right)
    static inline const uint64_t FILE_C = 0x0404040404040404ULL; // file C (third bit from right)
    static inline const uint64_t FILE_D = 0x0808080808080808ULL; // file D (fourth bit from right)
    static inline const uint64_t FILE_E = 0x1010101010101010ULL; // file E (fifth bit from right)
    static inline const uint64_t FILE_F = 0x2020202020202020ULL; // file F (sixth bit from right)
    static inline const uint64_t FILE_G = 0x4040404040404040ULL; // file G (seventh bit from right)
    static inline const uint64_t FILE_H = 0x8080808080808080ULL; // file H (leftmost bit in each byte)

    // Starting position masks for rooks
    static inline const uint64_t WHITE_ROOK_KINGSIDE_START = 0x0000000000000080ULL;  // h1
    static inline const uint64_t WHITE_ROOK_QUEENSIDE_START = 0x0000000000000001ULL;  // a1
    static inline const uint64_t BLACK_ROOK_KINGSIDE_START = 0x8000000000000000ULL;  // h8
    static inline const uint64_t BLACK_ROOK_QUEENSIDE_START = 0x0100000000000000ULL;  // a8

    // Starting position masks for kings
    static inline const uint64_t WHITE_KING_START = 0x0000000000000010ULL;  // e1
    static inline const uint64_t BLACK_KING_START = 0x1000000000000000ULL;  // e8

    // Castling path masks (squares that must be empty and not under attack)
    static inline const uint64_t WHITE_KINGSIDE_CASTLING_PATH = 0x0000000000000060ULL;  // f1 and g1
    static inline const uint64_t WHITE_QUEENSIDE_CASTLING_PATH = 0x000000000000000EULL;  // b1, c1, and d1
    static inline const uint64_t BLACK_KINGSIDE_CASTLING_PATH = 0x6000000000000000ULL;  // f8 and g8
    static inline const uint64_t BLACK_QUEENSIDE_CASTLING_PATH = 0x0E00000000000000ULL;  // b8, c8, and d8

    // Castling masks for rook and king end positions
    static inline const uint64_t WHITE_KINGSIDE_CASTLING_KING_END = 0x0000000000000040ULL;  // g1
    static inline const uint64_t WHITE_QUEENSIDE_CASTLING_KING_END = 0x0000000000000004ULL;  // c1
    static inline const uint64_t BLACK_KINGSIDE_CASTLING_KING_END = 0x4000000000000000ULL;  // g8
    static inline const uint64_t BLACK_QUEENSIDE_CASTLING_KING_END = 0x0400000000000000ULL;  // c8

    static inline const uint64_t WHITE_KINGSIDE_CASTLING_ROOK_END = 0x0000000000000020ULL;  // f1
    static inline const uint64_t WHITE_QUEENSIDE_CASTLING_ROOK_END = 0x0000000000000008ULL;  // d1
    static inline const uint64_t BLACK_KINGSIDE_CASTLING_ROOK_END = 0x2000000000000000ULL;  // f8
    static inline const uint64_t BLACK_QUEENSIDE_CASTLING_ROOK_END = 0x0800000000000000ULL;  // d8

    struct Magic
    {
        uint64_t mask;
        uint64_t magic;
        int shift;
        //std::span<const uint64_t> tableView;
        size_t offset;  // offset into attackTable
    };

}
