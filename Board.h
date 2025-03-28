#pragma once
#include "Common.h"
#include "Engine/Chess.h"
#include "Engine/Ai.h"
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
    Rectangle m_boardRect = Rectangle();

    std::vector<Chess::Board> m_moveHistory;

    float m_boardTexelSize = 0;

    bool m_currentPlayerIsWhite = true; //inverted each step
    bool m_playerIsWhite = true;
    glm::ivec2 m_chosenPiece = glm::ivec2(-1, -1); //the piece the player chose

    std::unordered_multimap<int, Chess::Board> m_nextBoards;
public:
    Board() {};

    bool choosePiece(glm::ivec2 pos) {
        if (pos == glm::ivec2(-1, -1)) return false;

        if (m_playerIsWhite) {
            pos.y = 7 - pos.y;
        }
        else pos.x = 7 - pos.x;
        auto type = m_board.getPieceAtSquare(pos.y * 8 + pos.x);
        if (type == Chess::PieceTypes::EMPTY ||
            m_currentPlayerIsWhite && type > Chess::PieceTypes::WHITE_KING ||
           !m_currentPlayerIsWhite && type < Chess::PieceTypes::BLACK_PAWN) {
            m_chosenPiece = glm::ivec2(-1, -1);
            return false;
        }
        else {
            m_chosenPiece = pos;
            //we dont care what move, just that it exists
            const auto& move = m_nextBoards.find(pos.y * 8 + pos.x);
            if (move == m_nextBoards.end())
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
        const auto& [boardNextBegin, boardNextEnd] = m_nextBoards.equal_range(
            m_chosenPiece.y * 8 + m_chosenPiece.x);

        for (auto it = boardNextBegin; it != boardNextEnd; ++it) {
            auto move = it->second.getLastMove();
            if (move.toSquare == pos.y * 8 + pos.x)
            {
                m_moveHistory.push_back(m_board);
                m_board = it->second;
                m_currentPlayerIsWhite = !m_currentPlayerIsWhite;

                if (m_currentPlayerIsWhite)
                    m_nextBoards = std::move(Chess::Calculator::getNextBoardsWhiteMultimap(m_board));
                else
                    m_nextBoards = std::move(Chess::Calculator::getNextBoardsBlackMultimap(m_board));
                return true;
            }
        }
        return false;
    }
        
    bool makeMove(const Chess::Board& nextBoard)
    {
        auto move = nextBoard.getLastMove();
        const auto& [boardNextBegin, boardNextEnd] = m_nextBoards.equal_range(move.fromSquare);

        for (auto& nextBoardIt : m_nextBoards) {
            if (nextBoardIt.second.getBitBoard().getAllPieces() != nextBoard.getBitBoard().getAllPieces())
                continue;
            m_moveHistory.push_back(m_board);
            m_board = nextBoardIt.second;
            m_currentPlayerIsWhite = !m_currentPlayerIsWhite;

            if (m_currentPlayerIsWhite)
                m_nextBoards = std::move(Chess::Calculator::getNextBoardsWhiteMultimap(m_board));
            else
                m_nextBoards = std::move(Chess::Calculator::getNextBoardsBlackMultimap(m_board));
            return true;
        }
        __debugbreak();
        return false;
    }

    bool revert()
    {
        if (m_moveHistory.size() == 0)
            return false;

        m_board = m_moveHistory.back();
        m_moveHistory.pop_back();
        m_currentPlayerIsWhite = !m_currentPlayerIsWhite;
        if (m_currentPlayerIsWhite)
            m_nextBoards = std::move(Chess::Calculator::getNextBoardsWhiteMultimap(m_board));
        else
            m_nextBoards = std::move(Chess::Calculator::getNextBoardsBlackMultimap(m_board));
        return true;
    }

    bool revertDouble()
    {
        if (m_moveHistory.size() < 2)
            return false;

        m_moveHistory.pop_back();
        m_board = m_moveHistory.back();
        m_moveHistory.pop_back();
        if (m_currentPlayerIsWhite)
            m_nextBoards = std::move(Chess::Calculator::getNextBoardsWhiteMultimap(m_board));
        else
            m_nextBoards = std::move(Chess::Calculator::getNextBoardsBlackMultimap(m_board));
        return true;
    }

    void startNewGame(bool isWhite) {
        m_board.reset();
        m_playerIsWhite = isWhite;
        m_currentPlayerIsWhite = true;
        m_nextBoards = Chess::Calculator::getNextBoardsWhiteMultimap(m_board);
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

    const Chess::Board& getBoard() const { return m_board; };

    const bool& playerIsWhite() const { return m_playerIsWhite; };
    const bool& currentPlayerIsWhite() const { return m_currentPlayerIsWhite; };

    std::vector<std::pair<glm::ivec2, Chess::PieceTypes>> getPiecePositions() const
    {
        std::vector<std::pair<glm::ivec2, Chess::PieceTypes>> positions;
        positions.reserve(32);

        if (m_playerIsWhite) //inverted because screen y is opposite to board y
        {
            for (int i = 1; i < static_cast<size_t>(Chess::PieceTypes::NUM); i++)
            {
                auto mask = m_board.getBitBoard().getPieceMask(static_cast<Chess::PieceTypes>(i));
                while (mask)
                {
                    int square = std::countr_zero(mask);
                    positions.push_back(std::make_pair(glm::ivec2(square % 8, 7 - square / 8),
                        static_cast<Chess::PieceTypes>(i)));
                    mask &= mask - 1;
                }
            }
        }
        else
        {
            for (int i = 1; i < static_cast<size_t>(Chess::PieceTypes::NUM); i++)
            {
                auto mask = m_board.getBitBoard().getPieceMask(static_cast<Chess::PieceTypes>(i));
                while (mask)
                {
                    int square = std::countr_zero(mask);
                    positions.push_back(std::make_pair(glm::ivec2(7 - square % 8, square / 8),
                        static_cast<Chess::PieceTypes>(i)));
                    mask &= mask - 1;
                }
            }
        }

        return positions;
    }

    bool onLMBPress(const Mouse& mouse) //returns true on move
    {
        if (m_chosenPiece == glm::ivec2(-1, -1))
            choosePiece(getTileFromMousePos(glm::ivec2(mouse.coordX, mouse.coordY)));
        else
        {
            bool moved = movePiece(getTileFromMousePos(glm::ivec2(mouse.coordX, mouse.coordY)));
            m_chosenPiece = glm::ivec2(-1, -1);
            return moved;
        }
        return false;
    }

    std::vector<glm::ivec2> getMovePositions() const // to
    {
        if (m_chosenPiece == glm::ivec2(-1, -1))
            return std::vector<glm::ivec2>();

        const auto& [boardNextBegin, boardNextEnd] = m_nextBoards.equal_range(
            m_chosenPiece.y * 8 + m_chosenPiece.x);

        std::vector<glm::ivec2> positions;
        positions.reserve(32);
        if (m_playerIsWhite)
            for (auto it = boardNextBegin; it != boardNextEnd; ++it) {
                auto move = it->second.getLastMove();
                positions.push_back(glm::ivec2(move.toSquare % 8, 7 - move.toSquare / 8));
            }
        else
            for (auto it = boardNextBegin; it != boardNextEnd; ++it) {
                auto move = it->second.getLastMove();
                positions.push_back(glm::ivec2(7 - (move.toSquare % 8), move.toSquare / 8));
            }

        return positions;
    }

    glm::ivec2 getChosePiecePosition() const // from
    {
        if (m_playerIsWhite)
            return glm::ivec2(m_chosenPiece.x, 7 - m_chosenPiece.y);
        else return glm::ivec2(7 - m_chosenPiece.x, m_chosenPiece.y);
    }

    bool shouldContinue() const //returns false if game is finished
    {
        if (!m_nextBoards.empty())
            return true;

        if (m_board.isWhiteChecked())
            return false; //white check mate

        else if (m_board.isBlackChecked())
            return false; //black check mate

        else return false; //stalemate

        return true;
    }
};

