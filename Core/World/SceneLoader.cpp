#include "SceneLoader.h"
#include <nlohmann/json.hpp>
#include "../ECS/SpriteAnimationComponent.h"
#include "../ECS/TransformComponent.h"
#include "../ECS/ColliderComponent.h"
#include "../ECS/SpriteComponent.h"
#include "../ECS/MovementComponent.h"
#include "../ECS/ControllerComponent.h"
#include "../ECS/TagComponent.h"
#include "../ECS/CameraComponent.h"
#include "../ECS/Registry.h"
#include "../Engine.h"
#include "TileMap.h"
#include <SDL2/SDL.h>
#include <fstream>
#include <unordered_map>

using Json = nlohmann::json;

namespace
{
	bool TryParseAnimation(const std::string& animationStr, AnimationState& outState)
	{
		static const std::unordered_map<std::string, AnimationState> animationMap
		{
			{ "Idle", AnimationState::Idle },
			{ "Walking", AnimationState::Walking },
			{ "Running", AnimationState::Running },
			{ "Jumping", AnimationState::Jumping },
			{ "Falling", AnimationState::Falling },
			{ "Attacking", AnimationState::Attacking },
			{ "Dying", AnimationState::Dying }
		};

		const auto it = animationMap.find(animationStr);
		if (it == animationMap.end())
		{
			return false;
		}

		outState = it->second;
		return true;
	}

	bool LoadTextureDefinitions(const Json& document, Engine& engine, SceneLoadResult& result)
	{
		if (!document.contains("textures"))
		{
			return true;
		}

		if (!document["textures"].is_array())
		{
			SDL_Log("Scene file 'textures' must be an array.");
			return false;
		}

		for (const Json& textureDef : document["textures"])
		{
			if (!textureDef.contains("name") || !textureDef.contains("filePath"))
			{
				SDL_Log("Texture definition is missing 'name' or 'filePath'.");
				return false;
			}

			const std::string name = textureDef["name"].get<std::string>();
			const std::string filePath = textureDef["filePath"].get<std::string>();

			if (!engine.GetTextureManager().LoadTexture(name, filePath))
			{
				SDL_Log("Failed to load texture '%s' from '%s'.", name.c_str(), filePath.c_str());
				return false;
			}

			result.loadedTextures.push_back(name);
		}

		return true;
	}

	bool LoadSoundDefinitions(const Json& document, Engine& engine, SceneLoadResult& result)
	{
		if (!document.contains("sounds"))
		{
			return true;
		}

		if (!document["sounds"].is_array())
		{
			SDL_Log("Scene file 'sounds' must be an array.");
			return false;
		}

		for (const Json& soundDef : document["sounds"])
		{
			if (!soundDef.contains("name") || !soundDef.contains("filePath"))
			{
				SDL_Log("Sound definition is missing 'name' or 'filePath'.");
				return false;
			}

			const std::string name = soundDef["name"].get<std::string>();
			const std::string filePath = soundDef["filePath"].get<std::string>();

			if (!engine.GetSoundManager().LoadSound(name, filePath))
			{
				SDL_Log("Failed to load sound '%s' from '%s'.", name.c_str(), filePath.c_str());
				return false;
			}

			result.loadedSounds.push_back(name);
		}

		return true;
	}

	bool LoadMusicDefinitions(const Json& document, Engine& engine, SceneLoadResult& result)
	{
		if (!document.contains("music"))
		{
			return true;
		}

		if (!document["music"].is_array())
		{
			SDL_Log("Scene file 'music' must be an array.");
			return false;
		}

		for (const Json& musicDef : document["music"])
		{
			if (!musicDef.contains("name") || !musicDef.contains("filePath"))
			{
				SDL_Log("Music definition is missing 'name' or 'filePath'.");
				return false;
			}

			const std::string name = musicDef["name"].get<std::string>();
			const std::string filePath = musicDef["filePath"].get<std::string>();

			if (!engine.GetSoundManager().LoadMusic(name, filePath))
			{
				SDL_Log("Failed to load music '%s' from '%s'.", name.c_str(), filePath.c_str());
				return false;
			}

			result.loadedMusic.push_back(name);
		}

		return true;
	}

	bool LoadTileMapDefinitions(const Json& document, Engine& engine, TileMap& tileMap)
	{
		if (!document.contains("tileMap"))
		{
			return true;
		}

		if (!document["tileMap"].is_object())
		{
			SDL_Log("Scene file 'tileMap' must be an object.");
			return false;
		}

		const Json& tileMapDef = document["tileMap"];

		if (!tileMapDef.contains("filePath") || !tileMapDef["filePath"].is_string())
		{
			SDL_Log("Tile map definition is missing 'filePath'.");
			return false;
		}

		const std::string filePath = tileMapDef["filePath"].get<std::string>();
		if (!tileMap.LoadFromTmj(filePath, engine.GetTextureManager()))
		{
			SDL_Log("Failed to load tile map '%s'.", filePath.c_str());
			return false;
		}

		return true;
	}

	void ApplyTransformComponent(const Json& entityDef, Entity& entity)
	{
		if (!entityDef.contains("transform") || !entityDef["transform"].is_object())
		{
			return;
		}

		const Json& transformDef = entityDef["transform"];

		entity.Add<TransformComponent>(
			transformDef.value("x", 0.0f),
			transformDef.value("y", 0.0f),
			transformDef.value("rotation", 0.0f),
			transformDef.value("scaleX", 1.0f),
			transformDef.value("scaleY", 1.0f));
	}

	void ApplySpriteComponent(const Json& entityDef, Entity& entity)
	{
		if (!entityDef.contains("sprite") || !entityDef["sprite"].is_object())
		{
			return;
		}

		const Json& spriteDef = entityDef["sprite"];

		if (!spriteDef.contains("texture") || !spriteDef["texture"].is_string())
		{
			SDL_Log("Sprite definition is missing 'texture'.");
			return;
		}

		const std::string textureName = spriteDef["texture"].get<std::string>();
		const int width = spriteDef.value("width", 0);
		const int height = spriteDef.value("height", 0);

		if (width <= 0 || height <= 0)
		{
			SDL_Log("Sprite definition has invalid width or height.");
			return;
		}

		SpriteComponent* sprite = nullptr;

		if (spriteDef.contains("sourceRect") && spriteDef["sourceRect"].is_object())
		{
			const Json& sourceRectDef = spriteDef["sourceRect"];

			SDL_Rect sourceRect
			{
				.x = sourceRectDef.value("x", 0),
				.y = sourceRectDef.value("y", 0),
				.w = sourceRectDef.value("w", 0),
				.h = sourceRectDef.value("h", 0)
			};

			sprite = &entity.Add<SpriteComponent>(textureName, sourceRect, width, height);
		}
		else
		{
			sprite = &entity.Add<SpriteComponent>(textureName, width, height);
		}

		sprite->m_isVisible = spriteDef.value("isVisible", sprite->m_isVisible);
	}

	void ApplyControllerComponent(const Json& entityDef, Entity& entity)
	{
		if (!entityDef.contains("controller") || !entityDef["controller"].is_object())
		{
			return;
		}

		entity.Add<ControllerComponent>();
	}

	void ApplyAnimationComponent(const Json& entityDef, Entity& entity)
	{
		if (!entityDef.contains("animations") || !entityDef["animations"].is_object())
		{
			return;
		}

		auto& animation = entity.Add<SpriteAnimationComponent>();

		for (const auto& [stateName, clipJson] : entityDef["animations"].items())
		{
			if (!clipJson.is_object())
			{
				SDL_Log("Animation clip for '%s' is not an object.", stateName.c_str());
				continue;
			}

			AnimationState state;
			if (!TryParseAnimation(stateName, state))
			{
				SDL_Log("Unknown animation state '%s'.", stateName.c_str());
				continue;
			}

			AnimationClip clip;
			clip.frameWidth = clipJson.value("frameWidth", 0);
			clip.frameHeight = clipJson.value("frameHeight", 0);
			clip.frameCount = clipJson.value("frameCount", 1);
			clip.frameDuration = clipJson.value("frameDuration", 0.1f);
			clip.row = clipJson.value("row", 0);
			clip.startFrame = clipJson.value("startFrame", 0);
			clip.isLooping = clipJson.value("isLooping", true);
			clip.textureName = clipJson.value("textureName", "");

			animation.m_animations[state] = clip;
		}
	}

	void ApplyTagComponent(const Json& entityDef, Entity& entity)
	{
		if (!entityDef.contains("tag") || !entityDef["tag"].is_string())
		{
			return;
		}

		entity.Add<TagComponent>(entityDef["tag"].get<std::string>());
	}

	void ApplyColliderComponent(const Json& entityDef, Entity& entity)
	{
		if (!entityDef.contains("collider") || !entityDef["collider"].is_object())
		{
			return;
		}

		const Json& colliderDef = entityDef["collider"];
		const int width = colliderDef.value("width", 0);
		const int height = colliderDef.value("height", 0);

		if (width <= 0 || height <= 0)
		{
			SDL_Log("Collider definition has invalid width or height.");
			return;
		}

		auto& collider = entity.Add<ColliderComponent>(
			width,
			height,
			colliderDef.value("offsetX", 0),
			colliderDef.value("offsetY", 0),
			colliderDef.value("isStatic", false));

		collider.m_isTrigger = colliderDef.value("isTrigger", false);
	}

	void ApplyCameraComponent(const Json& entityDef, Entity& entity)
	{
		if (!entityDef.contains("camera") || !entityDef["camera"].is_object())
		{
			return;
		}

		const Json& cameraDef = entityDef["camera"];

		auto& camera = entity.Add<CameraComponent>(
			cameraDef.value("x", 0),
			cameraDef.value("y", 0),
			cameraDef.value("width", 800),
			cameraDef.value("height", 600));

		camera.m_shouldFollow = cameraDef.value("shouldFollow", true);
		camera.m_offsetX = cameraDef.value("offsetX", 0);
		camera.m_offsetY = cameraDef.value("offsetY", 0);
	}

	void ApplyMovementComponent(const Json& entityDef, Entity& entity)
	{
		if (!entityDef.contains("movement") || !entityDef["movement"].is_object())
		{
			return;
		}

		const Json& movementDef = entityDef["movement"];
		auto& movement = entity.Add<MovementComponent>();

		if (!movementDef.contains("moveSpeed"))
		{
			SDL_Log("Movement component doesn't contain 'moveSpeed' property");
			return;
		}

		movement.moveSpeed = movementDef.value("moveSpeed", movement.moveSpeed);
	}

	bool LoadEntities(const Json& document, Registry& registry, SceneLoadResult& result)
	{
		if (!document.contains("entities") || !document["entities"].is_array())
		{
			SDL_Log("Scene file is missing 'entities' array or it is not an array.");
			return false;
		}

		for (const Json& entityDef : document["entities"])
		{
			if (!entityDef.is_object())
			{
				SDL_Log("Entity definition is not an object.");
				return false;
			}

			Entity entity = registry.CreateEntity();

			ApplyTransformComponent(entityDef, entity);
			ApplyControllerComponent(entityDef, entity);
			ApplyMovementComponent(entityDef, entity);
			ApplySpriteComponent(entityDef, entity);
			ApplyAnimationComponent(entityDef, entity);
			ApplyTagComponent(entityDef, entity);
			ApplyColliderComponent(entityDef, entity);
			ApplyCameraComponent(entityDef, entity);

			/*if (entity.IsValid())
			{
				const std::string tag = entity.Get<TagComponent>() ? entity.Get<TagComponent>()->Tag : "Entity" + std::to_string(entity.GetId());
				result.entities[tag] = entity;
			}*/

			const std::string key = entityDef.value("key", "");
			if (!key.empty() && entity.IsValid())
			{
				result.entities[key] = entity;
			}
			else
			{
				const TagComponent* tagComponent = entity.Get<TagComponent>();
				const std::string tag = tagComponent ? tagComponent->Tag : "Entity" + std::to_string(entity.GetId());
				result.entities[tag] = entity;
			}
		}

		return true;
	}
}

SceneLoadResult SceneLoader::LoadScene(const std::string& filePath, Engine& engine, Registry& registry, TileMap& tileMap)
{
	SceneLoadResult result;
	std::ifstream file(filePath);

	if (!file.is_open())
	{
		SDL_Log("Failed to open scene file: %s", filePath.c_str());
		return result;
	}

	Json document;
	
	try
	{
		file >> document;
	}
	catch (const Json::parse_error& e)
	{
		SDL_Log("Failed to parse scene file: %s. Error: %s", filePath.c_str(), e.what());
		return result;
	}

	if (!LoadTextureDefinitions(document, engine, result) ||
		!LoadSoundDefinitions(document, engine, result) ||
		!LoadMusicDefinitions(document, engine, result) ||
		!LoadTileMapDefinitions(document, engine, tileMap) ||
		!LoadEntities(document, registry, result))
	{
		SDL_Log("Failed to load scene from file: %s", filePath.c_str());
		return result;
	}

	result.loaded = true;
	return result;
}