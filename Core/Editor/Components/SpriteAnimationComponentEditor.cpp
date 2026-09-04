#include "SpriteAnimationComponentEditor.h"
#include "../../ECS/SpriteAnimationComponent.h"
#include <imgui.h>
#include "../../Managers/AnimationManager.h"
#include "../../Engine.h"

namespace
{
	constexpr const char* GetAnimationStateName(AnimationState state)
	{
		switch (state)
		{
		case AnimationState::Idle:
			return "Idle";
		case AnimationState::Walk:
			return "Walk";
		case AnimationState::Run:
			return "Run";
		case AnimationState::Jump:
			return "Jump";
		case AnimationState::Fall:
			return "Fall";
		case AnimationState::Attack:
			return "Attack";
		case AnimationState::Die:
			return "Die";
		default:
			return "Unknown";
		}
	}
}

bool SpriteAnimationComponentEditor::Render(Engine& engine, Entity& entity)
{
#ifdef _DEBUG
	SpriteAnimationComponent* animation = entity.Get<SpriteAnimationComponent>();

	if (animation == nullptr)
	{
		return false;
	}

	ImGui::SeparatorText("Sprite Animation");
	ImGui::PushID("SpriteAnimationComponent");

	bool changed = false;
	AnimationManager& animationManager = engine.GetAnimationManager();

	if (ImGui::BeginCombo("Animation Set", animation->animationSetName.empty() ? "<None>" : animation->animationSetName.c_str()))
	{
		for (const auto& [setName, set] : animationManager.GetAnimationSets())
		{
			(void)set;

			const bool isSelected = setName == animation->animationSetName;

			if (ImGui::Selectable(setName.c_str(), isSelected))
			{
				animation->animationSetName = setName;
				animation->currentFrame = 0;

				for (const AnimationState state :
				{
					AnimationState::Idle,
					AnimationState::Walk,
					AnimationState::Run,
					AnimationState::Jump,
					AnimationState::Fall,
					AnimationState::Attack,
					AnimationState::Die
				})
				{
					if (animationManager.GetAnimationDefinition(setName, state) != nullptr)
					{
						animation->currentAnimation = state;
						break;
					}
				}

				changed = true;
			}

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

	if (!animation->animationSetName.empty() && ImGui::BeginCombo("Current Animation", GetAnimationStateName(animation->currentAnimation)))
	{
		for (const AnimationState state :
		{
			AnimationState::Idle,
			AnimationState::Walk,
			AnimationState::Run,
			AnimationState::Jump,
			AnimationState::Fall,
			AnimationState::Attack,
			AnimationState::Die
		})
		{
			if (animationManager.GetAnimationDefinition(animation->animationSetName, state) == nullptr)
			{
				continue;
			}

			const bool isSelected = state == animation->currentAnimation;

			if (ImGui::Selectable(GetAnimationStateName(state), isSelected))
			{
				animation->currentAnimation = state;
				animation->currentFrame = 0;
				changed = true;
			}

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

	changed |= ImGui::DragFloat("Speed Multiplier", &animation->speedMultiplier, 0.01f, 0.0f, 10.0f);

	changed |= ImGui::Checkbox("Playing", &animation->isPlaying);

	ImGui::PopID();

	return changed;
#else
	(void)engine;
	(void)entity;
	return false;
#endif
}