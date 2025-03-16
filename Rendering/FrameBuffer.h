#pragma once
#include <GL/glew.h>
#include <string>
#include <array>

#include "Shader.h"

class FrameBuffer
{
public:

    enum class Buffers
    {
        POSITION,
        UV,
        INDEX,
        NUM,
    };

    enum class Locations
    {
        POSITION = 0,
        UV = 1,
    };

    static inline const std::array<glm::vec2, 4> positions = {
        glm::vec2(-1.0f, -1.0f),
        glm::vec2( 1.0f, -1.0f),
        glm::vec2( 1.0f,  1.0f),
        glm::vec2(-1.0f,  1.0f),
    };

    static inline const std::array<glm::vec2, 4> uvs = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f),
    };

    static inline const std::array<glm::ivec3, 2> indices = {
        glm::vec3(0, 1, 2),
        glm::vec3(0, 2, 3)
    };

private:
    GLuint m_frameBuffer;
    GLuint m_renderTexture;
    GLuint m_depthRenderBuffer;

    size_t m_width, m_height;
    GLuint m_vao;
    std::array<uint32_t, static_cast<size_t>(Buffers::NUM)> m_buffers;
    const Shader* m_shader;

    bool m_isInit = false;
public:
    FrameBuffer() : m_width(0), m_height(0), m_shader(nullptr), m_isInit(false) {};

    ~FrameBuffer() {
        if (m_isInit)
            deleteBuffers();
    };

    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;

    FrameBuffer(FrameBuffer&& other) noexcept = default;
    FrameBuffer& operator=(FrameBuffer&& other) noexcept = default;


    void bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
        glViewport(0, 0, m_width, m_height);  // Set viewport to framebuffer size
    }

    void unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void drawToScreen(int screenX, int screenY, int screenWidth, int screenHeight) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(screenX, screenY, screenWidth, screenHeight);

        m_shader->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_renderTexture);

        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    // Getters might be useful
    GLuint getTextureId() const { return m_renderTexture; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }  // Note: "height" is misspelled in your code

    void initializeFramebuffer(int width, int height, const Shader* shader) {
        if (m_isInit)
            deleteBuffers();

        m_width = width;
        m_height = height;
        m_shader = shader;

        // Create framebuffer
        glGenFramebuffers(1, &m_frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

        glGenRenderbuffers(1, &m_depthRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER, m_depthRenderBuffer);

        // Create texture to render to
        glGenTextures(1, &m_renderTexture);
        glBindTexture(GL_TEXTURE_2D, m_renderTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
            m_width, m_height,
            0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, m_renderTexture, 0);

        glGenVertexArrays(1, &m_vao);
        glGenBuffers(static_cast<size_t>(Buffers::NUM), m_buffers.data());

        // Bind VAO first
        glBindVertexArray(m_vao);

        // Setup position buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[static_cast<size_t>(Buffers::POSITION)]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * positions.size(), positions.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(static_cast<size_t>(Locations::POSITION), 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(static_cast<size_t>(Locations::POSITION));

        // Setup UV buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[static_cast<size_t>(Buffers::UV)]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvs.size(), uvs.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(static_cast<size_t>(Locations::UV), 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(static_cast<size_t>(Locations::UV));

        // Setup element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[static_cast<size_t>(Buffers::INDEX)]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * indices.size(), indices.data(), GL_STATIC_DRAW);

        // Unbind VAO
        glBindVertexArray(0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("Framebuffer is not complete!");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_isInit = true;
    }

    void deleteBuffers()
    {
        glDeleteBuffers(static_cast<size_t>(Buffers::NUM), m_buffers.data());
        glDeleteVertexArrays(1, &m_vao);
        glDeleteTextures(1, &m_renderTexture);
        glDeleteFramebuffers(1, &m_frameBuffer);

        m_isInit = false;
    }
};

