#pragma once
#pragma warning(disable: 4996)

#define _CRT_SECURE_NO_WARNINGS

//vendor
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


#include "Utilities/ArrayNd.h"

//c++ std 
#include <iostream>
#include <optional>
#include <array>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <unordered_map>
//#define _DEBUG

struct KeyState
{
	bool state; //true - pressed, false - released
	bool isChanged;
};

struct Rectangle
{
	size_t width;
	size_t height;

	size_t offsetX;
	size_t offsetY;
};

struct ComparatorIvec2 {
	bool operator()(const glm::ivec2& a, const glm::ivec2& b) const {
		if (a.x != b.x) return a.x < b.x;
		return a.y < b.y;
	}
};

namespace std {
	template<>
	struct hash<glm::ivec2> {
		size_t operator()(const glm::ivec2& v) const {
			return hash<int>()(v.x) ^ (hash<int>()(v.y) << 1);
		}
	};
}