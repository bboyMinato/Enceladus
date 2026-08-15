#include "SpriteAnimationSystem.h"
#include <algorithm>

void AnimationSystem::Update(Registry& registry, AnimationManager& animationManager, float deltaTime)
{
	registry.ForEach<SpriteComponent, SpriteAnimationComponent>(
		[&](Entity entity, SpriteComponent& sprite, SpriteAnimationComponent& animation)
		{
			UpdateSprite(sprite, animation, animationManager, deltaTime);
		}
	);
}

void AnimationSystem::UpdateSprite(SpriteComponent& sprite, SpriteAnimationComponent& animation, AnimationManager& animationManager, float deltaTime)
{
	const AnimationDefinition* animDef = animationManager.GetAnimationDefinition(animation.animationSetName, animation.currentAnimation);

	if (!animDef || !animDef->IsValid())
	{
		return;
	}

	// Update sprite source rect from shared definition
	sprite.m_hasSourceRect = true;
	sprite.m_sourceRect.w = animDef->frameWidth;
	sprite.m_sourceRect.h = animDef->frameHeight;
	sprite.m_sourceRect.y = animDef->row * animDef->frameHeight;

	// Update sprite texture name if defined in the animation definition
	if (!animDef->textureName.empty())
	{
		sprite.m_textureName = animDef->textureName;
	}

	if (animation.isPlaying && animDef->frameDuration > 0.0f)
	{
		const float adjustedDeltaTime = deltaTime * animation.speedMultiplier;
		animation.elapsedTime += adjustedDeltaTime;

		// Prevent performance issues by limiting the number of frame advances in a single update
		const int maxAdvances = 10;
		int advances = 0;

		while (animation.elapsedTime >= animDef->frameDuration && advances < maxAdvances)
		{
			animation.elapsedTime -= animDef->frameDuration;
			++animation.currentFrame;
			
			if (animation.currentFrame >= animDef->frameCount)
			{
				if (animDef->isLooping)
				{
					animation.currentFrame = 0;
				}
				else
				{
					animation.currentFrame = animDef->frameCount - 1;
					animation.isPlaying = false;

					break; // Stop advancing frames if the animation is not looping
				}
			}

			++advances;
		}
	}

	animation.currentFrame = std::clamp(animation.currentFrame, 0, animDef->frameCount - 1);
	sprite.m_sourceRect.x = (animDef->startFrame + animation.currentFrame) * animDef->frameWidth;
}