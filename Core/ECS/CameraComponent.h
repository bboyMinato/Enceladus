#pragma once 
#include "../Math/Vector2.h"
#include "Entity.h"
#include <SDL2/SDL.h>
#include <nlohmann/json.hpp>

enum class CameraMode
{
	Follow,
	Free,
	Fixed
};

NLOHMANN_JSON_SERIALIZE_ENUM(CameraMode,
{
	{ CameraMode::Follow, "follow" },
	{ CameraMode::Free, "free" },
	{ CameraMode::Fixed, "fixed" }
})

struct CameraComponent
{
	Entity m_target{};

	SDL_Rect m_viewport{ 0, 0, 800, 600 };
	
	Vector2f m_followOffset{ 0.0f, 0.0f };
	float m_zoom{ 1.0f };

	CameraMode m_mode{ CameraMode::Follow }; 

	bool m_shouldFollow{ true };
	bool m_isActive{ true };
};