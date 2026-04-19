#include "AnimationStateSystem.h"
#include "../ECS/SpriteComponent.h"
#include "../ECS/SpriteAnimationComponent.h"
#include "../ECS/MovementComponent.h"

void AnimationStateSystem::UpdateAnimationStates(Registry& registry)
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

                sprite.m_flip =(sprite.m_facingDirection == FacingDirection::Right)
                                 ? SDL_FLIP_HORIZONTAL
                                 : SDL_FLIP_NONE;

                if (isMoving)
                {
                    AnimationUtils::PlayAnimation(sprite, anim, AnimationState::Walking);
                }
                else
                {
                    AnimationUtils::PlayAnimation(sprite, anim, AnimationState::Idle);
                }
            }
        );
}