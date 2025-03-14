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
        BLACK_BISHOP,
        BLACK_ROOK,
        BLACK_QUEEN,
        BLACK_KING
    };

    struct PieceData
    {
        Piece piece;
        size_t moveCounter;
        size_t lastMoved;

        PieceData() : piece(Piece::EMPTY), moveCounter(0), lastMoved(0) {};
        PieceData(Piece piece) : piece(piece), moveCounter(0), lastMoved(0) {};


    };

    static inline const size_t BOARD_WIDTH = 142; //in pixels
    static inline const size_t BOARD_HEIGHT = 142;
    static inline const size_t BOARD_HIGHLIGHT_WIDTH = 7;
    static inline const size_t BOARD_TILE_SIZE = 16; //matches the piece size exactly

private:
    Utils::ArrayNd<Piece, 8, 8> m_board; //for fast lookup
    /*std::multimap<Piece, glm::ivec2> m_piecePositions;*/
    std::map<glm::ivec2, PieceData, ComparatorIvec2> m_blackPieces;
    std::map<glm::ivec2, PieceData, ComparatorIvec2> m_whitePieces;

    Rectangle m_boardRect;
    float m_boardTexelSize;

    size_t m_totalStepCount = 0;
    bool m_currentPlayerIsWhite = true; //inverted each step
    bool m_playerIsWhite = true;
    glm::ivec2 chosenPiece = glm::ivec2(-1, -1); //the piece the player chose

public:
    Board() {};

    bool chosePiece(glm::ivec2 pos) {
        if (pos == glm::ivec2(-1, -1) ||
            m_board.at(pos.x, pos.y) == Piece::EMPTY ||
            m_currentPlayerIsWhite && m_board.at(pos.x, pos.y) > Piece::WHITE_KING ||
           !m_currentPlayerIsWhite && m_board.at(pos.x, pos.y) < Piece::BLACK_PAWN) {
            chosenPiece = glm::ivec2(-1, -1);
            return false;
        }
        else {
            chosenPiece = pos;
            return true;
        }
    }

    bool movePiece(glm::ivec2 pos) {
        if (chosenPiece == glm::ivec2(-1, -1)) {
            return false;
        }
        else {
            m_board.at(pos.x, pos.y) = m_board.at(chosenPiece.x, chosenPiece.y);
            m_board.at(chosenPiece.x, chosenPiece.y) = Piece::EMPTY;
            m_totalStepCount++;
            m_currentPlayerIsWhite = !m_currentPlayerIsWhite;
            return true;
        }
    }

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
        size_t queenColumn;
        size_t kingColumn;

        if (isWhite)
        {
            whiteRow = 7;
            whitePawnRow = 6;
            blackRow = 0;
            blackPawnRow = 1;
            queenColumn = 3;
            kingColumn = 4;
        }
        else
        {
            whiteRow = 0;
            whitePawnRow = 1;
            blackRow = 7;
            blackPawnRow = 6;
            queenColumn = 4;
            kingColumn = 3;
        }
       
        m_board.at(0, whiteRow) = Piece::WHITE_ROOK;
        m_whitePieces.emplace(glm::ivec2(0, whiteRow), Piece::WHITE_ROOK);
        m_board.at(1, whiteRow) = Piece::WHITE_KNIGHT;
        m_whitePieces.emplace(glm::ivec2(1, whiteRow), Piece::WHITE_KNIGHT);
        m_board.at(2, whiteRow) = Piece::WHITE_BISHOP;
        m_whitePieces.emplace(glm::ivec2(2, whiteRow), Piece::WHITE_BISHOP);
        m_board.at(queenColumn, whiteRow) = Piece::WHITE_QUEEN;
        m_whitePieces.emplace(glm::ivec2(queenColumn, whiteRow), Piece::WHITE_QUEEN);
        m_board.at(kingColumn, whiteRow) = Piece::WHITE_KING;
        m_whitePieces.emplace(glm::ivec2(kingColumn, whiteRow), Piece::WHITE_KING);
        m_board.at(5, whiteRow) = Piece::WHITE_BISHOP;
        m_whitePieces.emplace(glm::ivec2(5, whiteRow), Piece::WHITE_BISHOP);
        m_board.at(6, whiteRow) = Piece::WHITE_KNIGHT;
        m_whitePieces.emplace(glm::ivec2(6, whiteRow), Piece::WHITE_KNIGHT);
        m_board.at(7, whiteRow) = Piece::WHITE_ROOK;
        m_whitePieces.emplace(glm::ivec2(7, whiteRow), Piece::WHITE_ROOK);

        for (int i = 0; i < 8; i++) {
            m_board.at(i, whitePawnRow) = Piece::WHITE_PAWN;
            m_whitePieces.emplace(glm::ivec2(i, whitePawnRow), Piece::WHITE_PAWN);
        }


        m_board.at(0, blackRow) = Piece::BLACK_ROOK;
        m_blackPieces.emplace(glm::ivec2(0, blackRow), Piece::BLACK_ROOK);
        m_board.at(1, blackRow) = Piece::BLACK_KNIGHT;
        m_blackPieces.emplace(glm::ivec2(1, blackRow), Piece::BLACK_KNIGHT);
        m_board.at(2, blackRow) = Piece::BLACK_BISHOP;
        m_blackPieces.emplace(glm::ivec2(2, blackRow), Piece::BLACK_BISHOP);
        m_board.at(queenColumn, blackRow) = Piece::BLACK_QUEEN;
        m_blackPieces.emplace(glm::ivec2(queenColumn, blackRow), Piece::BLACK_QUEEN);
        m_board.at(kingColumn, blackRow) = Piece::BLACK_KING;
        m_blackPieces.emplace(glm::ivec2(kingColumn, blackRow), Piece::BLACK_KING);
        m_board.at(5, blackRow) = Piece::BLACK_BISHOP;
        m_blackPieces.emplace(glm::ivec2(5, blackRow), Piece::BLACK_BISHOP);
        m_board.at(6, blackRow) = Piece::BLACK_KNIGHT;
        m_blackPieces.emplace(glm::ivec2(6, blackRow), Piece::BLACK_KNIGHT);
        m_board.at(7, blackRow) = Piece::BLACK_ROOK;
        m_blackPieces.emplace(glm::ivec2(7, blackRow), Piece::BLACK_ROOK);

        for (int i = 0; i < 8; i++) {
            m_board.at(i, blackPawnRow) = Piece::BLACK_PAWN;
            m_blackPieces.emplace(glm::ivec2(i, blackPawnRow), Piece::BLACK_PAWN);
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

    const std::map<glm::ivec2, PieceData, ComparatorIvec2>& getWhitePieces() const { return m_whitePieces; };
    const std::map<glm::ivec2, PieceData, ComparatorIvec2>& getBlackPieces() const { return m_blackPieces; };

    static inline bool isWhite(Piece piece) { return piece < Piece::BLACK_PAWN; };

    template<Piece P>
    inline std::vector<glm::ivec2> getLegalMoves(const glm::ivec2& from) const;

    template<>
    inline std::vector<glm::ivec2> getLegalMoves<Piece::WHITE_PAWN>(const glm::ivec2& from) const {
        return getPawnMoves(from);
    }

    template<>
    inline std::vector<glm::ivec2> getLegalMoves<Piece::BLACK_PAWN>(const glm::ivec2& from) const {
        return getPawnMoves(from);
    }

private:

    std::vector<glm::ivec2> getPawnMoves(const glm::ivec2& from) const; //might separate by color in the future


};

