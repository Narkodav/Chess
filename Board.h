#pragma once
#include "Common.h"
#include "Chess.h"

class Board
{
public:

    static inline const size_t BOARD_WIDTH = 142; //in pixels
    static inline const size_t BOARD_HEIGHT = 142;
    static inline const size_t BOARD_HIGHLIGHT_WIDTH = 7;
    static inline const size_t BOARD_TILE_SIZE = 16; //matches the piece size exactly

private:
    Chess::Board m_board;
    Rectangle m_boardRect;
    float m_boardTexelSize;

    bool m_currentPlayerIsWhite = true; //inverted each step
    bool m_playerIsWhite = true;
    glm::ivec2 chosenPiece = glm::ivec2(-1, -1); //the piece the player chose

public:
    Board() {};

    //bool chosePiece(glm::ivec2 pos) {
    //    if (pos == glm::ivec2(-1, -1) ||
    //        m_board.at(pos.x, pos.y).piece == Piece::EMPTY ||
    //        m_currentPlayerIsWhite && m_board.at(pos.x, pos.y).piece > Piece::WHITE_KING ||
    //       !m_currentPlayerIsWhite && m_board.at(pos.x, pos.y).piece < Piece::BLACK_PAWN) {
    //        chosenPiece = glm::ivec2(-1, -1);
    //        return false;
    //    }
    //    else {
    //        chosenPiece = pos;
    //        return true;
    //    }
    //}

    //bool movePiece(glm::ivec2 pos) {
    //    if (chosenPiece == glm::ivec2(-1, -1)) {
    //        return false;
    //    }
    //    else {
    //        m_board.at(pos.x, pos.y) = m_board.at(chosenPiece.x, chosenPiece.y);
    //        m_board.at(chosenPiece.x, chosenPiece.y) = Piece::EMPTY;
    //        m_totalStepCount++;
    //        m_currentPlayerIsWhite = !m_currentPlayerIsWhite;
    //        return true;
    //    }
    //}

    void setupInitialPosition() {
        m_board.reset();
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

};

