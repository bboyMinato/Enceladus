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
                    PlayAnimation(sprite, anim, animationManager, "walk");
                }
                else
                {
                    PlayAnimation(sprite, anim, animationManager, "idle");
                }
            }
        );
}

void AnimationStateSystem::PlayAnimation(SpriteComponent& sprite, SpriteAnimationComponent& anim, AnimationManager& animationManager, std::string_view animationName)
{
    if (anim.currentAnimation == animationName && anim.isPlaying)
    {
        return;
    }

    const AnimationDefinition* animDef = animationManager.GetAnimationDefinition(anim.animationSetName, animationName);
    if (!animDef || !animDef->IsValid())
    {
        return;
    }

    if (!animDef->textureName.empty())
    {
        sprite.m_textureName = animDef->textureName;
    }

    anim.currentAnimation = animationName;
    anim.currentFrame = 0;
    anim.elapsedTime = 0.0f;
    anim.isPlaying = true;
}

void AnimationStateSystem::SetSpeed(SpriteAnimationComponent& anim, float speedMultiplier)
{
    anim.speedMultiplier = std::max(0.0f, speedMultiplier);
}