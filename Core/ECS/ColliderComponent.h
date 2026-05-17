#pragma once

struct ColliderComponent
{
	ColliderComponent() = default;

	ColliderComponent(int width, int height)
		: m_width(width), m_height(height)
	{
	}

	ColliderComponent(int width, int height, int offsetX, int offsetY, bool isStatic = false)
		: m_width(width), m_height(height), m_offsetX(offsetX), m_offsetY(offsetY), m_isStatic(isStatic)
	{
	}

	int m_width{ 0 };
	int m_height{ 0 };

	int m_offsetX{ 0 };
	int m_offsetY{ 0 };

	bool m_isTrigger{ false };
	bool m_isStatic{ false };
};