#include "SpriteAnimationSystem.h"

void SpriteAnimationSystem::Update(Registry& registry, float deltaTime)
{
	registry.ForEach<SpriteComponent, SpriteAnimationComponent>(
		[&](Entity entity, SpriteComponent& sprite, SpriteAnimationComponent& animation)
		{
			UpdateSprite(sprite, animation, deltaTime);
		}
	);
}

void SpriteAnimationSystem::UpdateSprite(SpriteComponent& sprite, SpriteAnimationComponent& animation, float deltaTime)
{
	if (animation.m_frameWidth <= 0 || animation.m_frameHeight <= 0 || animation.m_frameCount <= 0)
	{
		return;
	}

	sprite.m_hasSourceRect = true;
	sprite.m_sourceRect.w = animation.m_frameWidth;
	sprite.m_sourceRect.h = animation.m_frameHeight;
	sprite.m_sourceRect.y = animation.m_row * animation.m_frameHeight;

	if (animation.m_isPlaying && animation.m_frameDuration > 0.0f)
	{
		animation.m_elapsedTime += deltaTime;

		while (animation.m_elapsedTime >= animation.m_frameDuration)
		{
			animation.m_elapsedTime -= animation.m_frameDuration;
			++animation.m_currentFrame;

			if (animation.m_currentFrame >= animation.m_frameCount)
			{
				if (animation.m_isLooping)
				{
					animation.m_currentFrame = 0;
				}
				else
				{
					animation.m_currentFrame = animation.m_frameCount - 1;
					animation.m_isPlaying = false;
					animation.m_elapsedTime = 0.0f;
					break;
				}
			}
		}
	}

	sprite.m_sourceRect.x = (animation.m_startFrame + animation.m_currentFrame) * animation.m_frameWidth;
}