#include "AnimationUtils.h"
#include "AnimationClip.h"
#include "../ECS/SpriteAnimationComponent.h"
#include "../ECS/SpriteComponent.h"

void AnimationUtils::PlayAnimation(SpriteComponent& sprite, SpriteAnimationComponent& anim, AnimationState state)
{
	if (anim.m_currentState == state && anim.m_isPlaying)
	{
		return;
	}

	auto it = anim.m_animations.find(state);
	if (it == anim.m_animations.end())
	{
		return;
	}

	if (it->second.textureName.empty())
	{
		return;
	}

	anim.m_currentState = state;

	// swap texture (important for multiple sheets)
	sprite.m_textureName = it->second.textureName;

	anim.m_currentFrame = 0;
	anim.m_elapsedTime = 0.0f;
	anim.m_isPlaying = true;
}