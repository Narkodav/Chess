#pragma once
#include "Common.h"

class Board
{
public:
	enum class Piece : size_t
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
        BLACK_BISHOP ,
        BLACK_ROOK ,
        BLACK_QUEEN,
        BLACK_KING
	};

    static inline const size_t BOARD_WIDTH = 142; //in pixels
    static inline const size_t BOARD_HEIGHT = 142;
    static inline const size_t BOARD_HIGHLIGHT_WIDTH = 7;
    static inline const size_t BOARD_TILE_SIZE = 16; //matches the piece size exactly

private:
    Utils::ArrayNd<Piece, 8, 8> m_board;
    std::multimap<Piece, glm::ivec2> m_piecePositions;

    Rectangle m_boardRect;

    float m_boardTexelSize;

public:
    Board() {};

    void setupInitialPosition(bool isWhite) {

        // Set empty squares in the middle
        for (int i = 0; i < 8; i++) {
            for (int j = 2; j < 6; j++) {
                m_board.at(i, j) = Piece::EMPTY;
            }
        }

        size_t whiteRow;
        size_t blackRow;
        size_t whitePawnRow;
        size_t blackPawnRow;

        if (isWhite)
        {
            whiteRow = 7;
            whitePawnRow = 6;
            blackRow = 0;
            blackPawnRow = 1;
        }
        else
        {
            whiteRow = 0;
            whitePawnRow = 1;
            blackRow = 7;
            blackPawnRow = 6;
        }
       
        m_board.at(0, whiteRow) = Piece::WHITE_ROOK;
        m_piecePositions.emplace(Piece::WHITE_ROOK, glm::ivec2(0, whiteRow));
        m_board.at(1, whiteRow) = Piece::WHITE_KNIGHT;
        m_piecePositions.emplace(Piece::WHITE_KNIGHT, glm::ivec2(1, whiteRow));
        m_board.at(2, whiteRow) = Piece::WHITE_BISHOP;
        m_piecePositions.emplace(Piece::WHITE_BISHOP, glm::ivec2(2, whiteRow));
        m_board.at(3, whiteRow) = Piece::WHITE_QUEEN;
        m_piecePositions.emplace(Piece::WHITE_QUEEN, glm::ivec2(3, whiteRow));
        m_board.at(4, whiteRow) = Piece::WHITE_KING;
        m_piecePositions.emplace(Piece::WHITE_KING, glm::ivec2(4, whiteRow));
        m_board.at(5, whiteRow) = Piece::WHITE_BISHOP;
        m_piecePositions.emplace(Piece::WHITE_BISHOP, glm::ivec2(5, whiteRow));
        m_board.at(6, whiteRow) = Piece::WHITE_KNIGHT;
        m_piecePositions.emplace(Piece::WHITE_KNIGHT, glm::ivec2(6, whiteRow));
        m_board.at(7, whiteRow) = Piece::WHITE_ROOK;
        m_piecePositions.emplace(Piece::WHITE_ROOK, glm::ivec2(7, whiteRow));

        for (int i = 0; i < 8; i++) {
            m_board.at(i, whitePawnRow) = Piece::WHITE_PAWN;
            m_piecePositions.emplace(Piece::WHITE_PAWN, glm::ivec2(i, whitePawnRow));
        }


        m_board.at(0, blackRow) = Piece::BLACK_ROOK;
        m_piecePositions.emplace(Piece::BLACK_ROOK, glm::ivec2(0, blackRow));
        m_board.at(1, blackRow) = Piece::BLACK_KNIGHT;
        m_piecePositions.emplace(Piece::BLACK_KNIGHT, glm::ivec2(1, blackRow));
        m_board.at(2, blackRow) = Piece::BLACK_BISHOP;
        m_piecePositions.emplace(Piece::BLACK_BISHOP, glm::ivec2(2, blackRow));
        m_board.at(3, blackRow) = Piece::BLACK_QUEEN;
        m_piecePositions.emplace(Piece::BLACK_QUEEN, glm::ivec2(3, blackRow));
        m_board.at(4, blackRow) = Piece::BLACK_KING;
        m_piecePositions.emplace(Piece::BLACK_KING, glm::ivec2(4, blackRow));
        m_board.at(5, blackRow) = Piece::BLACK_BISHOP;
        m_piecePositions.emplace(Piece::BLACK_BISHOP, glm::ivec2(5, blackRow));
        m_board.at(6, blackRow) = Piece::BLACK_KNIGHT;
        m_piecePositions.emplace(Piece::BLACK_KNIGHT, glm::ivec2(6, blackRow));
        m_board.at(7, blackRow) = Piece::BLACK_ROOK;
        m_piecePositions.emplace(Piece::BLACK_ROOK, glm::ivec2(7, blackRow));

        for (int i = 0; i < 8; i++) {
            m_board.at(i, blackPawnRow) = Piece::BLACK_PAWN;
            m_piecePositions.emplace(Piece::BLACK_PAWN, glm::ivec2(i, blackPawnRow));
        }
    }

    // Helper function for mouse interaction, returns -1, -1 if no tile selected
    glm::ivec2 getTileFromMousePos(const glm::ivec2& mousePos) const
    {
        if (mousePos.x < m_boardRect.offsetX + m_boardTexelSize * BOARD_HIGHLIGHT_WIDTH
            || mousePos.x > m_boardRect.offsetX + m_boardRect.width - m_boardTexelSize * BOARD_HIGHLIGHT_WIDTH
            || mousePos.y < m_boardRect.offsetY + m_boardTexelSize * BOARD_HIGHLIGHT_WIDTH
            || mousePos.y > m_boardRect.offsetY + m_boardRect.height - m_boardTexelSize * BOARD_HIGHLIGHT_WIDTH)
            return glm::ivec2(-1, -1);

        glm::ivec2 tilePos;
        tilePos.x = (mousePos.x - m_boardRect.offsetX - m_boardTexelSize * BOARD_HIGHLIGHT_WIDTH) / (BOARD_TILE_SIZE * m_boardTexelSize);
        tilePos.y = (mousePos.y - m_boardRect.offsetY - m_boardTexelSize * BOARD_HIGHLIGHT_WIDTH) / (BOARD_TILE_SIZE * m_boardTexelSize);
        return tilePos;
    }

    //called on window resize
    void setBoardRect(float windowWidth, float windowHeight)
    {
        if (windowWidth > windowHeight)
        {
            m_boardRect.offsetY = 0;
            m_boardRect.offsetX = (windowWidth - windowHeight) / 2;
            m_boardRect.height = windowHeight;
            m_boardRect.width = windowHeight;
        }
        else
        {
            m_boardRect.offsetY = (windowHeight - windowWidth) / 2;
            m_boardRect.offsetX = 0;
            m_boardRect.height = windowWidth;
            m_boardRect.width = windowWidth;
        }

        m_boardTexelSize = (float)m_boardRect.width / BOARD_WIDTH;
    }

    const Rectangle& getBoardRect() const { return m_boardRect; };

    Rectangle getTileRect(glm::ivec2 coord, float windowWidth, float windowHeight) const
    {
        Rectangle rect;

        rect.offsetX = m_boardRect.offsetX + m_boardTexelSize * BOARD_HIGHLIGHT_WIDTH + coord.x * m_boardTexelSize * BOARD_TILE_SIZE;
        rect.offsetY = m_boardRect.offsetY + m_boardTexelSize * BOARD_HIGHLIGHT_WIDTH + coord.y * m_boardTexelSize * BOARD_TILE_SIZE;

        rect.height = m_boardTexelSize * BOARD_TILE_SIZE;
        rect.width = m_boardTexelSize * BOARD_TILE_SIZE;

        return rect;
    }

    const std::multimap<Piece, glm::ivec2>& getPiecePositions() const { return m_piecePositions; };

};

