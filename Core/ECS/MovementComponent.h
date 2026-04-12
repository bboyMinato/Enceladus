#pragma once
#include "../Math/Vector2.h"

struct MovementComponent
{
	Vector2f velocity{};
	float moveSpeed{ 300.0f };
};