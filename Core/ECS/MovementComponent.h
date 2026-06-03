#pragma once
#include "../Math/Vector2.h"

struct MovementComponent
{
	Vector2f velocity{ 0, 0 };
	float moveSpeed{ 200.0f };

#ifdef _DEBUG
	bool m_useManualMovement{ false };
#endif
};