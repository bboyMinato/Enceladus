#pragma once

struct SpriteComponent;
struct SpriteAnimationComponent;
enum class AnimationState;

class AnimationUtils
{
public:
	static void PlayAnimation(SpriteComponent& sprite, SpriteAnimationComponent& anim, AnimationState state);
};