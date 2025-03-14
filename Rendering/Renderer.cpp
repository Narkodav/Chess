#include "Renderer.h"


void Renderer::draw(const Board& board, const Mouse& mouse, 
    GLFWwindow* windowHandle, size_t windowWidth, size_t windowHeight, float deltaTime)
{
    glClearColor(AssetRepository::backgroundColor.x,
        AssetRepository::backgroundColor.y,
        AssetRepository::backgroundColor.z,
        AssetRepository::backgroundColor.w);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //ImGui_ImplOpenGL3_NewFrame();
    //ImGui_ImplGlfw_NewFrame();
    //ImGui::NewFrame();

    m_frameBuffer.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto& shader = m_assets.getShader(AssetRepository::Shaders::FLAT_TO_SCREEN_WITH_PROJECTION);

    shader.bind();
    shader.setUniformMat4f("uProjection", m_projection);
    shader.setUniform1f("uZValue", 0.0f);

    drawBoard();

    m_highlightTime += deltaTime;

    if (m_shouldHighlight)
    {
        float alpha = m_baseAlpha - (m_pulseAmount * (std::sin(m_highlightTime * m_pulseSpeed) + 1.0f) * 0.5f);
        const auto& shaderHighlight = m_assets.getShader(AssetRepository::Shaders::PULSATING_HIGHLIGHT);
        shaderHighlight.bind();
        shaderHighlight.setUniform1f("uAlpha", alpha);
        shaderHighlight.setUniformMat4f("uProjection", m_projection);
        shaderHighlight.setUniform1f("uZValue", 0.1f);
        drawHighlight();
    }

    shader.bind();
    shader.setUniformMat4f("uProjection", m_projection);
    shader.setUniform1f("uZValue", 0.2f);
    drawPieces();

    //ImGui::Render();
    //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (windowWidth > windowHeight)
        m_frameBuffer.drawToScreen((windowWidth - windowHeight) / 2, 0, windowHeight, windowHeight);
    else
        m_frameBuffer.drawToScreen(0, (windowHeight - windowWidth) / 2, windowWidth, windowWidth);

    glfwSwapBuffers(windowHandle);
}

void Renderer::drawBoard()
{
    m_boardMesh.draw();
}

void Renderer::drawPieces()
{
    for (const auto& piece : m_pieces) {
        piece.draw();
    }
}
void Renderer::drawHighlight()
{
    m_highlightMesh.draw();
}

void Renderer::setHighlight(const Board& board, const Mouse& mouse, size_t windowWidth, size_t windowHeight)
{
    glm::ivec2 cursorPos = board.getTileFromMousePos(glm::ivec2(mouse.coordX, mouse.coordY));
    //std::cout << "Cursor pos x: " << cursorPos.x << " y: " << cursorPos.y << std::endl;
    //std::cout << std::flush;

    if (cursorPos.x == -1)
    {
        m_shouldHighlight = false;
        return;
    }

    m_highlightMesh.setVertices({
            glm::vec2(Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * cursorPos.x,
                Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * (cursorPos.y + 1)),
            glm::vec2(Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * (cursorPos.x + 1),
                Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * (cursorPos.y + 1)),
            glm::vec2(Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * (cursorPos.x + 1),
                Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * cursorPos.y),
            glm::vec2(Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * cursorPos.x,
                Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * cursorPos.y) })
                .setupBuffers();
    m_shouldHighlight = true;
}

void Renderer::remeshScene(const Board& board, size_t windowWidth, size_t windowHeight)
{
    m_boardMesh.setVertices({
        glm::vec2(0, Board::BOARD_HEIGHT),
        glm::vec2(Board::BOARD_WIDTH, Board::BOARD_HEIGHT),
        glm::vec2(Board::BOARD_WIDTH, 0),
        glm::vec2(0, 0)
        })
        .setTexture(&m_assets.getTexture(AssetRepository::Textures::TEXTURE_BOARD))
        .setupBuffers();

    const auto& whitePieces = board.getWhitePieces();
    const auto& blackPieces = board.getBlackPieces();
    m_pieces.clear();
    m_pieces.reserve(whitePieces.size() + blackPieces.size());

    for (const auto& pair : whitePieces) {
        m_pieces.push_back(FlatTexture());
        m_pieces.back().setVertices({
            glm::vec2(Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * pair.first.x,
                Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * (pair.first.y + 1)),
            glm::vec2(Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * (pair.first.x + 1),
                Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * (pair.first.y + 1)),
            glm::vec2(Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * (pair.first.x + 1),
                Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * pair.first.y),
            glm::vec2(Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * pair.first.x,
                Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * pair.first.y) })
                .setTexture(&m_assets.getTexture(static_cast<AssetRepository::Textures>(pair.second.piece)))
            .setupBuffers();
    }
    for (const auto& pair : blackPieces) {
        m_pieces.push_back(FlatTexture());
        m_pieces.back().setVertices({
            glm::vec2(Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * pair.first.x,
                Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * (pair.first.y + 1)),
            glm::vec2(Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * (pair.first.x + 1),
                Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * (pair.first.y + 1)),
            glm::vec2(Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * (pair.first.x + 1),
                Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * pair.first.y),
            glm::vec2(Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * pair.first.x,
                Board::BOARD_HIGHLIGHT_WIDTH + Board::BOARD_TILE_SIZE * pair.first.y) })
                .setTexture(&m_assets.getTexture(static_cast<AssetRepository::Textures>(pair.second.piece)))
            .setupBuffers();
    }
}