#pragma once
#include "../Utility/AnimationUtils.h"
#include "../Utility/AnimationClip.h"
#include <unordered_map>

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
	SpriteAnimationComponent() = default;

	std::unordered_map<AnimationState, AnimationClip> m_animations;

	AnimationState m_currentState{ AnimationState::Idle };

	int m_currentFrame{ 0 };
	float m_elapsedTime{ 0.0f };

	bool m_isPlaying{ true };
};