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
    Rectangle m_boardRect;

    float m_boardTexelSize;

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
        auto type = m_board.getPieceTypeAtSquare(pos.y * 8 + pos.x);
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
            int to;
            //special checks for castling and en passant
            if (it->second.getBitBoard().whiteKing != m_board.getBitBoard().whiteKing 
            && m_currentPlayerIsWhite)
                to = Chess::Calculator::getFromToPair(
                    it->second.getBitBoard().whiteKing, m_board.getBitBoard().whiteKing).second;
            else if (it->second.getBitBoard().blackKing != m_board.getBitBoard().blackKing
                && !m_currentPlayerIsWhite)
                to = Chess::Calculator::getFromToPair(
                    it->second.getBitBoard().blackKing, m_board.getBitBoard().blackKing).second;
            else if (it->second.getBitBoard().whitePawns != m_board.getBitBoard().whitePawns
                && m_currentPlayerIsWhite)
                to = Chess::Calculator::getFromToPair(
                    it->second.getBitBoard().whitePawns, m_board.getBitBoard().whitePawns).second;
            else if (it->second.getBitBoard().blackPawns != m_board.getBitBoard().blackPawns
                && !m_currentPlayerIsWhite)
                to = Chess::Calculator::getFromToPair(
                    it->second.getBitBoard().blackPawns, m_board.getBitBoard().blackPawns).second;
            else if (m_currentPlayerIsWhite)
                to = Chess::Calculator::getFromToPairWhite(it->second, m_board).second;
            else to = Chess::Calculator::getFromToPairBlack(it->second, m_board).second;
            if (to == pos.y * 8 + pos.x)
            {
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
        std::pair<int, int> fromTo;
        if (m_currentPlayerIsWhite)
            fromTo = Chess::Calculator::getFromToPairWhite(nextBoard, m_board);
        else fromTo = Chess::Calculator::getFromToPairBlack(nextBoard, m_board);

        const auto& [boardNextBegin, boardNextEnd] = m_nextBoards.equal_range(fromTo.first);

        for (auto& nextBoardIt : m_nextBoards) {
            if (nextBoardIt.second.getBitBoard().getAllPieces() != nextBoard.getBitBoard().getAllPieces())
                continue;
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
            for (int x = 0; x < 8; x++)
                for (int y = 0; y < 8; y++)
                {
                    auto type = m_board.getPieceTypeAtSquare(y * 8 + x);
                    if (type != Chess::PieceTypes::EMPTY)
                        positions.push_back(std::make_pair(glm::ivec2(x, 7 - y), type));
                }
        else
            for (int x = 0; x < 8; x++)
                for (int y = 0; y < 8; y++)
                {
                    auto type = m_board.getPieceTypeAtSquare(y * 8 + x);
                    if (type != Chess::PieceTypes::EMPTY)
                        positions.push_back(std::make_pair(glm::ivec2(7 - x, y), type));
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
                int to;
                if (it->second.getBitBoard().whiteKing != m_board.getBitBoard().whiteKing
                    && m_currentPlayerIsWhite)
                    to = Chess::Calculator::getFromToPair(
                        it->second.getBitBoard().whiteKing, m_board.getBitBoard().whiteKing).second;
                else if (it->second.getBitBoard().whitePawns != m_board.getBitBoard().whitePawns
                    && m_currentPlayerIsWhite)
                    to = Chess::Calculator::getFromToPair(
                        it->second.getBitBoard().whitePawns, m_board.getBitBoard().whitePawns).second;
                else to = Chess::Calculator::getFromToPairWhite(it->second, m_board).second;
                positions.push_back(glm::ivec2(to % 8, 7 - to / 8));
            }
        else
            for (auto it = boardNextBegin; it != boardNextEnd; ++it) {
                int to;
                if (it->second.getBitBoard().blackKing != m_board.getBitBoard().blackKing
                    && !m_currentPlayerIsWhite)
                    to = Chess::Calculator::getFromToPair(
                        it->second.getBitBoard().blackKing, m_board.getBitBoard().blackKing).second;
                else if (it->second.getBitBoard().blackPawns != m_board.getBitBoard().blackPawns
                    && !m_currentPlayerIsWhite)
                    to = Chess::Calculator::getFromToPair(
                        it->second.getBitBoard().blackPawns, m_board.getBitBoard().blackPawns).second;
                else to = Chess::Calculator::getFromToPairBlack(it->second, m_board).second;
                positions.push_back(glm::ivec2(7 - (to % 8), to / 8));
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

