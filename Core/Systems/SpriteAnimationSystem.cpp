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
	auto it = animation.m_animations.find(animation.m_currentState);
	if (it == animation.m_animations.end())
	{
		return;
	}

	auto& clip = it->second;

	sprite.m_hasSourceRect = true;

	sprite.m_sourceRect.w = clip.frameWidth;
	sprite.m_sourceRect.h = clip.frameHeight;
	sprite.m_sourceRect.y = clip.row * clip.frameHeight;

	if (animation.m_isPlaying && clip.frameDuration > 0.0f)
	{
		animation.m_elapsedTime += deltaTime;

		while (animation.m_elapsedTime >= clip.frameDuration)
		{
			animation.m_elapsedTime -= clip.frameDuration;
			++animation.m_currentFrame;

			if (animation.m_currentFrame >= clip.frameCount)
			{
				if (clip.isLooping)
				{
					animation.m_currentFrame = 0;
				}
				else
				{
					animation.m_currentFrame = clip.frameCount - 1;
					animation.m_isPlaying = false;
					animation.m_elapsedTime = 0.0f;
					break;
				}
			}
		}
	}

	sprite.m_sourceRect.x = (clip.startFrame + animation.m_currentFrame) * clip.frameWidth;
}