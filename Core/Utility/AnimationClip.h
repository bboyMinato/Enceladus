#pragma once
#include <string>

struct AnimationClip
{
	int frameWidth{ 0 };
	int frameHeight{ 0 };
	int frameCount{ 1 };
	float frameDuration{ 0.1f };

	int row{ 0 };
	int startFrame{ 0 };

	bool isLooping{ true };

	std::string textureName;
};