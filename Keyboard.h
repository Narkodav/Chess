#pragma once
#include "Common.h"

struct Keyboard
{
	enum class Keys
	{
		KEY_W = 0,
		KEY_S,
		KEY_A,
		KEY_D,
		KEY_SPACE,
		KEY_SHIFT,
		KEY_ESC,
		KEY_PLAYER_ACTION_FLAG,
		KEY_COUNT,
	};

	std::array<KeyState, static_cast<size_t>(Keys::KEY_COUNT)> m_keys;

};

