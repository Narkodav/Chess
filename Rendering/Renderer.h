#pragma once
#include "Board.h"
#include "Mouse.h"
#include "AssetRepository.h"
#include "FlatTexture.h"

class Renderer
{
	AssetRepository m_assets;
	
	FlatTexture m_boardMesh;
	bool m_shouldHighlight = false;
	FlatTexture m_highlightMesh;

	std::vector<FlatTexture> m_pieces;
public:

	void draw(const Board& board, const Mouse& mouse, GLFWwindow* windowHandle);

	void loadAssets() { m_assets.loadAssets(); };

	void remeshScene(const Board& board, size_t windowWidth, size_t windowHeight);
	void setHighlight(const Board& board, const Mouse& mouse, size_t windowWidth, size_t windowHeight);

private:
	void drawBoard();
	void drawPieces();
	void drawHighlight();
};

