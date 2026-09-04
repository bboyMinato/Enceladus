#pragma once
#include "../ECS/SpriteAnimationComponent.h"
#include <filesystem>
#include <string>
#include <unordered_map>

class AnimationManager
{
public:
	/// <summary>
	/// Loads an animation set from a JSON file and stores it in the manager.
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	bool LoadAnimationSet(const std::filesystem::path& filePath);

	const AnimationDefinition* GetAnimationDefinition(std::string_view setName, AnimationState animationState) const;

	const AnimationSet* GetAnimationSet(std::string_view setName) const;

	const std::unordered_map<std::string, AnimationDefinition>& GetAnimations(std::string_view setName) const;

	const auto& GetAnimationSets() const { return m_animationSets; }

	void Clear();
private:
	std::unordered_map<std::string, AnimationSet> m_animationSets;
};