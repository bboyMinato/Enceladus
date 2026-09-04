#pragma once
#include <functional>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

enum class AnimationState
{
	Idle,
	Walk,
	Run,
	Jump,
	Fall,
	Attack,
	Die
};

NLOHMANN_JSON_SERIALIZE_ENUM(AnimationState, 
{
	{ AnimationState::Idle, "idle" },
	{ AnimationState::Walk, "walk" },
	{ AnimationState::Run, "run" },
	{ AnimationState::Jump, "jump" },
	{ AnimationState::Fall, "fall" },
	{ AnimationState::Attack, "attack" },
	{ AnimationState::Die, "die" }	
})

struct SpriteAnimationComponent
{
	std::string animationSetName;

	AnimationState currentAnimation{ AnimationState::Idle };

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