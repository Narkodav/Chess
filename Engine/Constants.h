#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <array>

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
}
