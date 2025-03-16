#pragma once
#include "Common.h"
#include "Rendering/Renderer.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Board.h"
#include "FrameRateCalculator.h"
#include "Multithreading/ThreadPool.h"
#include "Multithreading/Synchronized.h"

class Game
{
public:
	enum class State
	{
		MAIN_MENU,
		GAME_MODE_SELECT,
		COLOR_SELECT,
		PLAYING,
		PAUSED,
        GAME_OVER,
	};

private:

	State m_gameState = State::MAIN_MENU;
	GLFWwindow* m_window;

	Renderer m_renderer;
	MT::Synchronized<Board> m_board;
	Keyboard m_keyboard;
	Mouse m_mouse;
	FrameRateCalculator frameRateCalc;
	Chess::Ai m_ai;
	MT::ThreadPool m_threadPool; //for async ai
    size_t m_aiDepth = 5;
    bool m_vsAi = false;
    bool m_playerWon = false;


	float m_frameTime, m_runtime;
	float m_aspectRatio;
	int m_width, m_height;

	void processInputs();

	//static callbacks
	static void static_keyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void static_windowResizeCallback(GLFWwindow* window, int width, int height);
	static void static_mouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
	static void static_mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void static_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	//callbacks
	void keyPressCallback(int key, int scancode, int action, int mods);
	void windowResizeCallback(int width, int height);
	void mouseMoveCallback(double xpos, double ypos);
	void mouseScrollCallback(double xoffset, double yoffset);
	void mouseButtonCallback(int button, int action, int mods);

#ifdef _DEBUG
	static void APIENTRY static_glDebugOutput(unsigned int source, unsigned int type,
		unsigned int id, unsigned int severity,
		int length, const char* message, const void* userParam);
#endif // _DEBUG

	//key binds
	void close();

public:

	Game();
	~Game();

	Game(const Game&) = delete;
	Game operator=(const Game&) = delete;

	int run();

    void asyncMoveCallback(const Chess::Move& move) {
        m_board.getWriteAccess()->makeMove(move);
    }

	void renderMenus() {
		// Center window on screen
		ImGui::SetNextWindowPos(ImVec2(m_width / 2, m_height / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		switch (m_gameState) {
		case State::MAIN_MENU:
			renderMainMenu();
			break;
		case State::GAME_MODE_SELECT:
			renderGameModeMenu();
			break;
		case State::COLOR_SELECT:
			renderColorSelectMenu();
			break;
        case State::GAME_OVER:
            renderGameOverMenu();
            break;
		case State::PAUSED:
			renderPausedMenu();
			break;
		}
	}

    void renderMainMenu() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));

        ImGui::Begin("##MainMenu", nullptr,
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar);

        // Title
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 215, 0, 255));  // Gold color
        ImGui::SetWindowFontScale(2.0f);
        ImGui::Text("Chess Game");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // Buttons
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
        const float buttonWidth = 250.0f;
        const float buttonHeight = 40.0f;

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.4f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.4f, 0.5f, 1.0f));

        if (ImGui::Button("Start Game", ImVec2(buttonWidth, buttonHeight))) {
            m_gameState = State::GAME_MODE_SELECT;
        }

        if (ImGui::Button("Exit", ImVec2(buttonWidth, buttonHeight))) {
            close();
        }

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);

        ImGui::End();

    }

    void renderGameModeMenu() {
        ImGui::Begin("##Select Game Mode", nullptr,
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar);

        const float buttonWidth = 200.0f;
        const float buttonHeight = 40.0f;

        ImGui::Text("Choose Game Mode:");
        ImGui::Spacing();

        if (ImGui::Button("Play vs AI", ImVec2(buttonWidth, buttonHeight))) {
            m_vsAi = true;
            m_gameState = State::COLOR_SELECT;
        }

        if (ImGui::Button("Play vs Human", ImVec2(buttonWidth, buttonHeight))) {
            //m_vsAi = false;
            //m_gameState = State::PLAYING;
        }

        if (ImGui::Button("Back", ImVec2(buttonWidth, buttonHeight))) {
            m_gameState = State::MAIN_MENU;
        }

        ImGui::End();
    }

    void renderColorSelectMenu() {
        ImGui::Begin("##Select Color", nullptr,
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar);

        const float buttonWidth = 200.0f;
        const float buttonHeight = 40.0f;

        ImGui::Text("Choose your color:");
        ImGui::Spacing();

        if (ImGui::Button("Play as White", ImVec2(buttonWidth, buttonHeight))) {
            //startGame(true);
            m_board.getWriteAccess()->startNewGame(true);
            m_ai.reset(false, m_aiDepth);
            m_gameState = State::PLAYING;
        }

        if (ImGui::Button("Play as Black", ImVec2(buttonWidth, buttonHeight))) {
            auto access = m_board.getWriteAccess();
            access->startNewGame(false);
            m_ai.reset(true, m_aiDepth);

            // If player is black, AI should make first move
            m_ai.getBestMoveAsync(access->getBoard(), m_threadPool,
                [this](Chess::Move move) {asyncMoveCallback(move); });
            m_gameState = State::PLAYING;
        }

        if (ImGui::Button("Back", ImVec2(buttonWidth, buttonHeight))) {
            m_gameState = State::GAME_MODE_SELECT;
        }

        ImGui::End();
    }

    void renderGameOverMenu() {
        // Center the window
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        // Set window padding
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));

        ImGui::Begin("##GameOver", nullptr,
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar);

        // Game Over Text
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 215, 0, 255));  // Gold color
        ImGui::SetWindowFontScale(2.0f);
        ImGui::Text("Game Over");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // Win/Lose Message
        ImGui::PushStyleColor(ImGuiCol_Text, m_playerWon ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 0, 0, 255));
        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text(m_playerWon ? "You Won!" : "You Lost!");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();

        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        // Buttons
        const float buttonWidth = 250.0f;
        const float buttonHeight = 40.0f;

        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.4f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.4f, 0.5f, 1.0f));

        if (ImGui::Button("Play Again", ImVec2(buttonWidth, buttonHeight))) {
            // Reset game state and start new game
            m_gameState = State::GAME_MODE_SELECT;  // or whatever state you need
        }

        if (ImGui::Button("Exit to Menu", ImVec2(buttonWidth, buttonHeight))) {
            m_gameState = State::MAIN_MENU;
        }

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);  // Pop button text align and window padding
        ImGui::End();
    }

    void renderPausedMenu() {
        // Center the window
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        // Set window padding
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));

        ImGui::Begin("##PauseMenu", nullptr,
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar);

        // Pause Text
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 215, 0, 255));  // Gold color
        ImGui::SetWindowFontScale(2.0f);
        ImGui::Text("Game Paused");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // Buttons
        const float buttonWidth = 250.0f;
        const float buttonHeight = 40.0f;

        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.4f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.4f, 0.5f, 1.0f));

        if (ImGui::Button("Continue", ImVec2(buttonWidth, buttonHeight))) {
            m_gameState = State::PLAYING;  // Resume game
            m_ai.setPaused(false);
        }

        if (ImGui::Button("Reset Game", ImVec2(buttonWidth, buttonHeight))) {
            // Reset game with current settings
            m_ai.abortAndWait();
            auto access = m_board.getWriteAccess();
            access->startNewGame(access->playerIsWhite());
            m_ai.reset(!access->playerIsWhite(), m_aiDepth);

            // If player is black, AI should make first move
            if (!access->playerIsWhite())
                m_ai.getBestMoveAsync(access->getBoard(), m_threadPool,
                    [this](Chess::Move move) {asyncMoveCallback(move); });
            m_gameState = State::PLAYING;
        }

        if (ImGui::Button("Exit to Menu", ImVec2(buttonWidth, buttonHeight))) {
            m_ai.abortAndWait();
            m_gameState = State::MAIN_MENU;
        }

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);  // Pop button text align and window padding
        ImGui::End();
    }
};