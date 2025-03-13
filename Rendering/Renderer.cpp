#include "Renderer.h"


void Renderer::draw(const Board& board, const Mouse& mouse, GLFWwindow* windowHandle)
{
    glClearColor(AssetRepository::backgroundColor.x,
        AssetRepository::backgroundColor.y,
        AssetRepository::backgroundColor.z,
        AssetRepository::backgroundColor.w);

    glClear(GL_COLOR_BUFFER_BIT);

    //ImGui_ImplOpenGL3_NewFrame();
    //ImGui_ImplGlfw_NewFrame();
    //ImGui::NewFrame();

    const auto& boardTexture = m_assets.getTexture(AssetRepository::Textures::TEXTURE_BOARD);
    const auto& shader = m_assets.getShader(AssetRepository::Shaders::FLAT_TO_SCREEN);

    shader.bind();
    boardTexture.bind();

    drawBoard();
    drawPieces();
    if (m_shouldHighlight)
    {
        const auto& shaderHighlight = m_assets.getShader(AssetRepository::Shaders::PULSATING_HIGHLIGHT);
        shaderHighlight.bind();
        shaderHighlight.setUniform1f("uAlpha", 0.5);
        drawHighlight();
    }
    //ImGui::Render();
    //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
    std::cout << "Cursor pos x: " << cursorPos.x << " y: " << cursorPos.y << std::endl;
    std::cout << std::flush;

    if (cursorPos.x == -1)
    {
        m_shouldHighlight = false;
        return;
    }

    m_highlightMesh
        .setFromRect(board.getTileRect(cursorPos, windowWidth, windowHeight), windowWidth, windowHeight)
        .setupBuffers();
    m_shouldHighlight = true;
}

void Renderer::remeshScene(const Board& board, size_t windowWidth, size_t windowHeight)
{
    m_boardMesh.setFromRect(board.getBoardRect(), windowWidth, windowHeight)
        .setTexture(&m_assets.getTexture(AssetRepository::Textures::TEXTURE_BOARD))
        .setupBuffers();

    const auto& positions = board.getPiecePositions();
    m_pieces.clear();
    m_pieces.reserve(positions.size());

    for (const auto& pair : positions) {
        m_pieces.push_back(FlatTexture());
        m_pieces.back().setFromRect(board.getTileRect(pair.second, windowWidth, windowHeight),
            windowWidth, windowHeight)
            .setTexture(&m_assets.getTexture(static_cast<AssetRepository::Textures>(pair.first)))
            .setupBuffers();
    }
}