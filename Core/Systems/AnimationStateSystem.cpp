#include "AnimationStateSystem.h"
#include "../ECS/MovementComponent.h"

void AnimationStateSystem::UpdateAnimationStates(Registry& registry, AnimationManager& animationManager)
{
    registry.ForEach<SpriteComponent, SpriteAnimationComponent, MovementComponent>(
            [&](Entity entity,
                SpriteComponent& sprite,
                SpriteAnimationComponent& anim,
                MovementComponent& movement)
            {
                bool isMoving =
                    movement.velocity.x != 0.0f ||
                    movement.velocity.y != 0.0f;

                if (movement.velocity.x > 0.0f)
                {
                    sprite.m_facingDirection = FacingDirection::Right;
                }
                else if (movement.velocity.x < 0.0f)
                {
                    sprite.m_facingDirection = FacingDirection::Left;
                }

                sprite.m_flip = (sprite.m_facingDirection == FacingDirection::Right)
                                 ? SDL_FLIP_HORIZONTAL
                                 : SDL_FLIP_NONE;

#ifdef _DEBUG
                if (anim.m_useManualState)
                {
                    return;
                }
#endif               

                if (isMoving)
                {
                    PlayAnimation(sprite, anim, animationManager, AnimationState::Walk);
                }
                else
                {
                    PlayAnimation(sprite, anim, animationManager, AnimationState::Idle);
                }
            }
        );
}

void AnimationStateSystem::PlayAnimation(SpriteComponent& sprite, SpriteAnimationComponent& anim, AnimationManager& animationManager, AnimationState animationState)
{
	if (anim.currentAnimation == animationState && anim.isPlaying)
	{
		return;
	}
	const AnimationDefinition* animDef = animationManager.GetAnimationDefinition(anim.animationSetName, animationState);
	if (!animDef || !animDef->IsValid())
	{
		return;
	}
	if (!animDef->textureName.empty())
	{
		sprite.m_textureName = animDef->textureName;
	}
	anim.currentAnimation = animationState;
	anim.currentFrame = 0;
	anim.elapsedTime = 0.0f;
	anim.isPlaying = true;
}

void AnimationStateSystem::SetSpeed(SpriteAnimationComponent& anim, float speedMultiplier)
{
    anim.speedMultiplier = std::max(0.0f, speedMultiplier);
}