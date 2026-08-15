#include "AnimationManager.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <SDL2/SDL.h>

using Json = nlohmann::json;

bool AnimationManager::LoadAnimationSet(std::string_view filePath)
{
	std::ifstream file(filePath.data());
	if (!file.is_open())
	{
		SDL_Log("Failed to open animation set file: %s", filePath.data());
		return false;
	}

	Json document = Json::parse(file, nullptr, false);

	if (document.is_discarded())
	{
		SDL_Log("Failed to parse animation set JSON file: %s", filePath.data());
		return false;
	}

	if (!document.is_array())
	{
		SDL_Log("Animation set JSON must be an array of sets. %s", filePath.data());
		return false;
	}

	for (const Json& set : document)
	{
		if (!set.contains("animations") || !set["animations"].is_object())
		{
			SDL_Log("Animation set JSON must contain an 'animations' object. %s", filePath.data());
			return false;
		}

		AnimationSet animSet;
		animSet.name = set.value("name", "");
		animSet.defaultTexture = set.value("defaultTexture", "");

		for (const auto& [animName, animationDef] : set["animations"].items())
		{
			if (!animationDef.is_object())
			{
				SDL_Log("Animation definition for '%s' is not an object.", animName.c_str());

				continue;
			}

			AnimationDefinition animDef;
			animDef.name = animName;
			animDef.frameWidth = animationDef.value("frameWidth", 0);
			animDef.frameHeight = animationDef.value("frameHeight", 0);
			animDef.frameCount = animationDef.value("frameCount", 1);
			animDef.frameDuration = animationDef.value("frameDuration", 0.1f);
			animDef.row = animationDef.value("row", 0);
			animDef.startFrame = animationDef.value("startFrame", 0);
			animDef.isLooping = animationDef.value("isLooping", true);
			animDef.priority = animationDef.value("priority", 0);
			animDef.textureName = animationDef.value("textureName", "");

			if (!animDef.IsValid())
			{
				SDL_Log("Invalid animation definition for '%s'.", animName.c_str());

				continue;
			}

			animSet.animations[animName] = animDef;
		}
		
		m_animationSets[animSet.name] = std::move(animSet);
	}


	return true;
}

const AnimationDefinition* AnimationManager::GetAnimationDefinition(std::string_view setName, std::string_view animationName) const
{
	auto it = m_animationSets.find(std::string(setName));
	if (it != m_animationSets.end())
	{
		const auto& animationSet = it->second;
		auto animIt = animationSet.animations.find(std::string(animationName));

		if (animIt != animationSet.animations.end())
		{
			return &animIt->second;
		}
	}

	return nullptr;
}

const AnimationSet* AnimationManager::GetAnimationSet(std::string_view setName) const
{
	auto it = m_animationSets.find(std::string(setName));
	if (it != m_animationSets.end())
	{
		return &it->second;
	}

	return nullptr;
}

const std::unordered_map<std::string, AnimationDefinition>& AnimationManager::GetAnimations(std::string_view setName) const
{
	auto it = m_animationSets.find(std::string(setName));
	if (it != m_animationSets.end())
	{
		return it->second.animations;
	}

	return std::unordered_map<std::string, AnimationDefinition>();
}

void AnimationManager::Clear()
{
	m_animationSets.clear();
}