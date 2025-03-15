#pragma once
#include "Common.h"
#include "Rendering/Renderer.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Board.h"
#include "FrameRateCalculator.h"
#include "Multithreading/ThreadPool.h"

class Game
{
private:

	GLFWwindow* m_window;

	Renderer m_renderer;
	Board m_board;
	Keyboard m_keyboard;
	Mouse m_mouse;
	FrameRateCalculator frameRateCalc;
	Chess::Ai m_ai;
	MT::ThreadPool m_threadPool; //for async ai
	bool m_waitingForAi = false;
	std::promise<Chess::Move> m_promise;
	std::future<Chess::Move> m_future;

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

};