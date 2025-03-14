#include "Board.h"

std::vector<glm::ivec2> Board::getPawnMoves(const glm::ivec2& from) const {
    // no need to bound check from because its a callers responsibility to ensure it

    bool callerIsWhite = isWhite(m_board.at(from.x, from.y)); //determined dynamically to make it more flexible
    Piece enemyPawn;
    Piece enemyKing;
    std::pair<const std::map<glm::ivec2, PieceData, ComparatorIvec2>&,
        const std::map<glm::ivec2, PieceData, ComparatorIvec2>&> pieceMaps =
        callerIsWhite ? std::make_pair(m_whitePieces, m_blackPieces) :
        std::make_pair(m_blackPieces, m_whitePieces); //first is friendly second is enemy

    if (callerIsWhite)
    {
        enemyPawn = Piece::BLACK_PAWN;
        enemyKing = Piece::BLACK_KING;
    }
    else
    {
        enemyPawn = Piece::WHITE_PAWN;
        enemyKing = Piece::WHITE_KING;
    }
    std::vector<glm::ivec2> moves;
    moves.reserve(4);

    //En passant check
    auto itRight = pieceMaps.second.find(from + glm::ivec2(1, 0));
    auto itLeft = pieceMaps.second.find(from + glm::ivec2(-1, 0));

    //no need to bound check here because iterators cannot return pieces not on the board
    if (itRight != pieceMaps.second.end() &&
        itRight->second.piece == enemyPawn &&
        itRight->second.lastMoved == m_totalStepCount - 1 &&
        itRight->second.moveCounter == 1 &&
        from.x + 1 < 8 && from.y - 1 > -1) //check in case the game has custom positioning
        //no need to check if the tile is free because it cannot be occupied if the pawn have moved
    {
        moves.push_back(from + glm::ivec2(1, -1));

    }
    //else if because it is impossible to have two pawns move on the same iteration
    else if (itLeft != pieceMaps.second.end() &&
        itLeft->second.piece == enemyPawn &&
        itLeft->second.lastMoved == m_totalStepCount - 1 &&
        itLeft->second.moveCounter == 1 &&
        from.x - 1 > -1 && from.y - 1 > -1)
    {
        moves.push_back(from + glm::ivec2(-1, -1));
    }

    //regular move
    if (from.y - 1 > -1 && m_board.at(from.x, from.y - 1) == Piece::EMPTY)
    {
        moves.push_back(from + glm::ivec2(0, -1));
        if (from.y - 2 > -1 &&
            m_board.at(from.x, from.y - 2) == Piece::EMPTY &&
            pieceMaps.first.find(from)->second.moveCounter == 0)
            moves.push_back(from + glm::ivec2(0, -2));
    }

    //regular captures
    if (from.x - 1 > -1 && from.y - 1 > -1 && //no need for upper check because from is already in bounds
        m_board.at(from.x - 1, from.y - 1) != Piece::EMPTY &&
        m_board.at(from.x - 1, from.y - 1) != enemyKing &&
        isWhite(m_board.at(from.x - 1, from.y - 1)) != callerIsWhite)
        moves.push_back(from + glm::ivec2(-1, -1));

    if (from.x + 1 < 8 && from.y - 1 > -1 &&
        m_board.at(from.x + 1, from.y - 1) != Piece::EMPTY &&
        m_board.at(from.x + 1, from.y - 1) != enemyKing &&
        isWhite(m_board.at(from.x + 1, from.y - 1)) != callerIsWhite)
        moves.push_back(from + glm::ivec2(1, -1));

    return moves;
};

