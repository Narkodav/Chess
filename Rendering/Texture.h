#pragma once
#include <GL/glew.h>
#include <string>

#include "stb_image/stb_image.h"

class Texture
{
private:
	unsigned int m_ID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
public:
	Texture();
	Texture(const std::string& path);
	~Texture();

	Texture(const Texture& other) = default;
	Texture& operator=(const Texture& other) = default;

	Texture(Texture&& other) noexcept = default;
	Texture& operator=(Texture&& other) noexcept = default;

	bool operator==(Texture& other) const noexcept; //compares ids

	void load(const std::string& path);
	void clear();

	void bind(unsigned int slot = 0) const;
	void unbind() const;

	inline int getWidth() const { return m_Width; };
	inline int getHeight() const { return m_Height; };
	inline unsigned int getId() const { return m_ID; };
};
