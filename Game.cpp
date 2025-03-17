#include "Game.h"

#ifdef _DEBUG
void APIENTRY Game::static_glDebugOutput(unsigned int source, unsigned int type, unsigned int id, unsigned int severity,
    int length, const char* message, const void* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cerr << "---------------" << std::endl;
    std::cerr << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             std::cerr << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cerr << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cerr << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cerr << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cerr << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cerr << "Source: Other"; break;
    } std::cerr << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cerr << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "Type: Deprecated Behavior"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cerr << "Type: Undefined Behavior"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cerr << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cerr << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cerr << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cerr << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cerr << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cerr << "Type: Other"; break;
    } std::cerr << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cerr << "Severity: high"; __debugbreak(); break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cerr << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cerr << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cerr << "Severity: notification"; break;

    } std::cerr << std::endl;
    std::cerr << std::endl;
}
#endif // _DEBUG

void Game::static_keyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    static_cast<Game*>(glfwGetWindowUserPointer(window))->keyPressCallback(key, scancode, action, mods);
}

void Game::static_windowResizeCallback(GLFWwindow* window, int width, int height)
{
    static_cast<Game*>(glfwGetWindowUserPointer(window))->windowResizeCallback(width, height);
}

void Game::static_mouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
    static_cast<Game*>(glfwGetWindowUserPointer(window))->mouseMoveCallback(xpos, ypos);
}

void Game::static_mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    static_cast<Game*>(glfwGetWindowUserPointer(window))->mouseScrollCallback(xoffset, yoffset);
}

void Game::static_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    static_cast<Game*>(glfwGetWindowUserPointer(window))->mouseButtonCallback(button, action, mods);
}

//callbacks
void Game::windowResizeCallback(int width, int height)
{
    m_width = width;
    m_height = height;
    m_aspectRatio = (double)m_width / m_height;
    glViewport(0, 0, m_width, m_height);
    m_board.getWriteAccess()->setBoardRect(m_width, m_height);
}

void Game::mouseMoveCallback(double xpos, double ypos)
{
    m_mouse.updateMouseMovement(xpos, ypos, m_frameTime);
}

void Game::mouseScrollCallback(double xoffset, double yoffset)
{
   /* m_mouse.offsetY = yoffset;*/
    //m_fov -= yoffset * m_scrollSensitivity;
    //m_renderer->setProjection(m_aspectRatio, m_fov);
}

#define CHECK_KEY(glfwCode, customCode, virtualKeyState)     case glfwCode: \
m_keyboard.m_keys[static_cast<size_t>(customCode)].isChanged = (virtualKeyState != m_keyboard.m_keys[static_cast<size_t>(customCode)].state); \
m_keyboard.m_keys[static_cast<size_t>(customCode)].state = virtualKeyState; \
break \

void Game::keyPressCallback(int key, int scancode, int action, int mods)
{
    bool virtualKeyState = (action != GLFW_RELEASE);
    switch (key)
    {
        
        CHECK_KEY(GLFW_KEY_W, Keyboard::Keys::KEY_W, virtualKeyState);
        CHECK_KEY(GLFW_KEY_S, Keyboard::Keys::KEY_S, virtualKeyState);
        CHECK_KEY(GLFW_KEY_A, Keyboard::Keys::KEY_A, virtualKeyState);
        CHECK_KEY(GLFW_KEY_D, Keyboard::Keys::KEY_D, virtualKeyState);
        CHECK_KEY(GLFW_KEY_LEFT_SHIFT, Keyboard::Keys::KEY_SHIFT, virtualKeyState);
        CHECK_KEY(GLFW_KEY_SPACE, Keyboard::Keys::KEY_SPACE, virtualKeyState);
        CHECK_KEY(GLFW_KEY_ESCAPE, Keyboard::Keys::KEY_ESC, virtualKeyState);
    }
}

void Game::mouseButtonCallback(int button, int action, int mods)
{

    bool virtualKeyState = (action == GLFW_PRESS);
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
        m_mouse.LMB.isChanged = (virtualKeyState != m_mouse.LMB.state);
        m_mouse.LMB.state = virtualKeyState;
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
        m_mouse.RMB.isChanged = (virtualKeyState != m_mouse.RMB.state);
        m_mouse.RMB.state = virtualKeyState;
        break;
    case GLFW_MOUSE_BUTTON_3:
        m_mouse.scrollWheel.isChanged = (virtualKeyState != m_mouse.scrollWheel.state);
        m_mouse.scrollWheel.state = virtualKeyState;
        break;
    }
}

void Game::processInputs()
{
    if (m_keyboard.m_keys[static_cast<size_t>(Keyboard::Keys::KEY_ESC)].isChanged &&
        m_keyboard.m_keys[static_cast<size_t>(Keyboard::Keys::KEY_ESC)].state)
    {
        if(m_gameState == State::PAUSED)
        {
            m_gameState = State::PLAYING;
            m_ai.setPaused(false);
        }
        else if(m_gameState == State::PLAYING)
        {
            m_gameState = State::PAUSED;
            m_ai.setPaused(true);
        }
    }

    for (int i = 0; i < static_cast<size_t>(Keyboard::Keys::KEY_COUNT); i++)
        m_keyboard.m_keys[i].isChanged = false;

    if (m_gameState == State::PLAYING)
    {
        auto access = m_board.getWriteAccess();
        if (m_mouse.LMB.isChanged && m_mouse.LMB.state &&
            access->playerIsWhite() == access->currentPlayerIsWhite() &&
            access->onLMBPress(m_mouse) && access->shouldContinue())
        {
            m_ai.getBestMoveAsync(access->getBoard(), m_threadPool,
                [this](Chess::Board nextBoard) {asyncMoveCallback(nextBoard); });
        }
    }

    m_mouse.LMB.isChanged = false;
    m_mouse.RMB.isChanged = false;
    m_mouse.scrollWheel.isChanged = false;
}

Game::Game() : m_frameTime(0.f),
m_width(1000), m_height(800)
{
    m_mouse.mouseSensitivity = 0.01f;
    m_mouse.scrollSensitivity = 2.0f;
    if (!glfwInit())
    {
        std::cerr << "failed to load glfw" << std::endl;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#endif // __DEBUG

    /* Create a windowed mode window and its OpenGL context */
    m_window = glfwCreateWindow(m_width, m_height, "test", NULL, NULL);

    m_aspectRatio = (float)m_width / m_height;

    if (m_window == NULL)
    {
        glfwTerminate();
        std::cerr << "failed to create a window" << std::endl;
    }

    glfwSetWindowUserPointer(m_window, this);
    glfwSetKeyCallback(m_window, static_keyPressCallback);
    glfwSetWindowSizeCallback(m_window, static_windowResizeCallback);
    glfwSetCursorPosCallback(m_window, static_mouseMoveCallback);
    glfwSetScrollCallback(m_window, static_mouseScrollCallback);
    glfwSetMouseButtonCallback(m_window, static_mouseButtonCallback);
}

Game::~Game()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

void Game::close()
{
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

int Game::run()
{
    int loadDistance = 3;
    int numOfAllocatedThreads = /*std::thread::hardware_concurrency() * 0.7*/ 4;
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(0);

    if (glewInit() != GLEW_OK)
        std::cout << "glew failed to initiate" << std::endl;
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << version << std::endl;

#ifdef _DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    glDebugMessageCallback(Game::static_glDebugOutput, nullptr);
#endif //_DEBUG

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glViewport(0, 0, m_width, m_height);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark(); // or StyleColorsLight()
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    ImGui::GetStyle().ScaleAllSizes(1.0f);  // Adjust if needed for DPI

    auto currentTime = std::chrono::high_resolution_clock::now();
    float frameRateUpdateCounter = 0.f;
    bool playerWhite = true;

    {
        auto boardAccess = m_board.getWriteAccess();
        frameRateCalc.setFrameTimeBuffer(100);
        m_renderer.loadAssets();
        boardAccess->setBoardRect(m_width, m_height);
        //float accumulator = 0.f;
        m_runtime = 0.f;

        m_threadPool.init(1);
    }

    while (!glfwWindowShouldClose(m_window))
    {
        ImGui::GetIO().DisplaySize = ImVec2((float)m_width, (float)m_height);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glfwPollEvents();
        glClearColor(AssetRepository::backgroundColor.x,
            AssetRepository::backgroundColor.y,
            AssetRepository::backgroundColor.z,
            AssetRepository::backgroundColor.w);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInputs();

        if (m_gameState != State::PLAYING) {
            renderMenus();
        }
        else
        {
            auto access = m_board.getWriteAccess();
            if (!access->shouldContinue())
            {
                //playerWhite = !playerWhite;
                //m_board.startNewGame(playerWhite);
                //m_ai.reset(!playerWhite, m_aiDepth);
                //if (!playerWhite)
                //{
                //    m_promise = std::promise<Chess::Move>();
                //    m_future = m_promise.get_future();
                //    m_threadPool.pushTask([this]() {
                //        m_promise.set_value(m_ai.getBestMove(m_board.getBoard()));
                //        });
                //    m_waitingForAi = true;
                //}
                m_ai.abortAndWait();
                if (access->getBoard().isBlackChecked() == access->playerIsWhite())
                    m_playerWon = true;
                else m_playerWon = false;
                m_gameState = State::GAME_OVER;
            }

            auto newTime = std::chrono::high_resolution_clock::now();
            m_frameTime = std::chrono::duration_cast<std::chrono::duration<float>>(newTime - currentTime).count();
            m_runtime += m_frameTime;
            currentTime = newTime;

            if (m_runtime >= frameRateUpdateCounter)
            {
                frameRateCalc.addFrameTime(m_frameTime);
                frameRateCalc.updateFrameRate();
                //frameRateCalc.printFrameRate();
                frameRateUpdateCounter += 3;
            }

            m_renderer.remeshScene(*access, m_width, m_height);
            m_renderer.setHighlight(*access, m_mouse, m_width, m_height);
            m_renderer.draw(*access, m_mouse, m_window, m_width, m_height, m_frameTime);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(m_window);

    }

    m_threadPool.shutdown();
    return 0;
}