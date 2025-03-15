#pragma once
#include "Common.h"
#include "Chess.h"
#include "Mouse.h"

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
    glm::ivec2 m_chosenPiece = glm::ivec2(-1, -1); //the piece the player chose
    std::vector<Chess::Move> m_movesForChosenPiece;
public:
    Board() {};

    bool choosePiece(glm::ivec2 pos) {
        if (pos == glm::ivec2(-1, -1)) return false;

        if (m_playerIsWhite) {
            pos.y = 7 - pos.y;
        }
        else pos.x = 7 - pos.x;

        if (m_board.at(pos).piece == Chess::Piece::Type::EMPTY ||
            m_currentPlayerIsWhite && m_board.at(pos).isBlack() ||
           !m_currentPlayerIsWhite && m_board.at(pos).isWhite()) {
            m_chosenPiece = glm::ivec2(-1, -1);
            return false;
        }
        else {
            m_chosenPiece = pos;
            m_movesForChosenPiece = Chess::Calculator::getAllPossibleMoves(m_board, m_chosenPiece);
            if (m_movesForChosenPiece.empty())
            {
                m_chosenPiece = glm::ivec2(-1, -1);
                return false;
            }
            return true;
        }
    }

    bool movePiece(glm::ivec2 pos) {
        if (m_chosenPiece == glm::ivec2(-1, -1)) {
            return false;
        }

        if (m_playerIsWhite) {
            pos.y = 7 - pos.y;
        }
        else pos.x = 7 - pos.x;

        if (m_chosenPiece == pos)
            return false;

        for (auto& move : m_movesForChosenPiece)
            if (move.to == pos)
            {
                m_board.move(move);
                m_board.incrementMoveCount();
                m_currentPlayerIsWhite = !m_currentPlayerIsWhite;
                return true;
            }
        return false;
    }

    void startNewGame(bool isWhite) {
        m_board.reset();
        m_playerIsWhite = isWhite;
        m_currentPlayerIsWhite = true;
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

    std::vector<std::pair<glm::ivec2, Chess::Piece::Type>> getPiecePositions() const
    {
        std::vector<std::pair<glm::ivec2, Chess::Piece::Type>> positions;
        positions.reserve(32);
        if(m_playerIsWhite) //inverted because screen y is opposite to board y
            for (int x = 0; x < 8; x++)
                for (int y = 0; y < 8; y++)
                {
                    if (m_board.at(x, y).piece != Chess::Piece::Type::EMPTY)
                        positions.push_back(std::make_pair(glm::ivec2(x, 7 - y), m_board.at(x, y).piece));
                }
        else //inverted both x and y to rotate the board instead of mirroring it
            for (int x = 0; x < 8; x++)
                for (int y = 0; y < 8; y++)
                {
                    if (m_board.at(x, y).piece != Chess::Piece::Type::EMPTY)
                        positions.push_back(std::make_pair(glm::ivec2(7 - x, y), m_board.at(x, y).piece));
                }


        return positions;
    }

    void onLMBPress(const Mouse& mouse)
    {
        if (m_chosenPiece == glm::ivec2(-1, -1))
            choosePiece(getTileFromMousePos(glm::ivec2(mouse.coordX, mouse.coordY)));
        else
        {
            movePiece(getTileFromMousePos(glm::ivec2(mouse.coordX, mouse.coordY)));
            m_chosenPiece = glm::ivec2(-1, -1);
            m_movesForChosenPiece.clear();
        }
    }

    std::vector<glm::ivec2> getMovePositions() const // to
    {
        std::vector<glm::ivec2> pos;
        pos.reserve(m_movesForChosenPiece.size());
        if (m_playerIsWhite)
            for (auto& move : m_movesForChosenPiece)
                pos.push_back(glm::ivec2(move.to.x, 7 - move.to.y));
        else
            for (auto& move : m_movesForChosenPiece)
                pos.push_back(glm::ivec2(7 - move.to.x, move.to.y));
        return pos;
    }

    glm::ivec2 getChosePiecePosition() const // from
    {
        if (m_playerIsWhite)
            return glm::ivec2(m_chosenPiece.x, 7 - m_chosenPiece.y);
        else return glm::ivec2(7 - m_chosenPiece.x, m_chosenPiece.y);
    }
};

