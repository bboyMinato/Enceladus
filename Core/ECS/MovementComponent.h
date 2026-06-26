#pragma once
#include "../Math/Vector2.h"

struct MovementComponent
{
	MovementComponent() = default;
	MovementComponent(float velocityX, float velocityY, float moveSpeed)
		: velocity(velocityX, velocityY), moveSpeed(moveSpeed)
	{
	}

	MovementComponent(Vector2f velocity, float moveSpeed)
		: velocity(velocity), moveSpeed(moveSpeed)
	{
	}

	Vector2f velocity{ 0, 0 };
	float moveSpeed{ 200.0f };

#ifdef _DEBUG
	bool m_useManualMovement{ false };
#endif
};