#pragma once
#include <unordered_map>
#include <functional>
#include <string>

enum class AnimationState
{
	Idle,
	Walking,
	Running,
	Jumping,
	Falling,
	Attacking,
	Dying
};

struct SpriteAnimationComponent
{
	std::string animationSetName;

	std::string currentAnimation{ "idle" };

	int currentFrame{ 0 };
	float elapsedTime{ 0.0f };
	float speedMultiplier{ 1.0f };

	bool isPlaying{ true };

#ifdef _DEBUG
	bool m_useManualState{ false };
#endif
};

struct AnimationDefinition
{
	std::string name;

	int frameWidth{ 0 };
	int frameHeight{ 0 };
	int frameCount{ 1 };
	float frameDuration{ 0.1f };

	int row{ 0 };
	int startFrame{ 0 };

	bool isLooping{ true };
	int priority{ 0 };

	std::string textureName;

	[[nodiscard]]
	bool IsValid() const
	{
		return frameWidth > 0 && frameHeight > 0 && 
			   frameCount > 0 && frameDuration > 0.0f;
	}
};

struct AnimationSet
{
	std::string name;
	std::string defaultTexture;
	std::unordered_map<std::string, AnimationDefinition> animations;	
};