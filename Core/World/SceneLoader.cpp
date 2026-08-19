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
#include "../ECS/InteractableComponent.h"
#include "../Engine.h"
#include "../Events/Event.h"
#include "TileMap.h"
#include <SDL2/SDL.h>
#include <fstream>
#include <unordered_map>
#include <algorithm>

using Json = nlohmann::json;

namespace
{
	bool TryParseAnimation(std::string animationStr, AnimationState& outState)
	{
		std::transform(animationStr.begin(), animationStr.end(), animationStr.begin(),
			[](unsigned char c) { return std::tolower(c); });

		static const std::unordered_map<std::string, AnimationState> animationMap
		{
			{ "idle",		AnimationState::Idle },
			{ "walking",	AnimationState::Walking },
			{ "running",	AnimationState::Running },
			{ "jumping",	AnimationState::Jumping },
			{ "falling",	AnimationState::Falling },
			{ "attacking",	AnimationState::Attacking },
			{ "dying",		AnimationState::Dying }
		};

		const auto it = animationMap.find(animationStr);
		if (it == animationMap.end())
		{
			return false;
		}

		outState = it->second;
		return true;
	}

	InteractionType StringToInteractionType(std::string str)
	{
		std::transform(str.begin(), str.end(), str.begin(),
			[](unsigned char c) { return std::tolower(c); });

		static const std::unordered_map<std::string, InteractionType> map = {
			{ "open",     InteractionType::Open },
			{ "pickup",   InteractionType::Pickup },
			{ "activate", InteractionType::Activate },
			{ "dialogue", InteractionType::Dialogue },
			{ "examine",  InteractionType::Examine } 
		};

		auto it = map.find(str);
		if (it != map.end())
		{
			return it->second;
		}

		SDL_Log("Unknown interactionType '%s', defaulting", str.c_str());
		return InteractionType::Default;
	}

	SDL_Scancode StringToScanCode(const std::string& name)
	{
		SDL_Keycode keycode = SDL_GetKeyFromName(name.c_str());

		if (keycode == SDLK_UNKNOWN)
		{
			SDL_Log("Unknown key name '%s', defaulting to SDLK_UNKNOWN", name.c_str());
			return SDL_SCANCODE_UNKNOWN;
		}

		return SDL_GetScancodeFromKey(keycode);
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

		const std::filesystem::path filePath = tileMapDef["filePath"].get<std::string>();
		
		std::optional<TileMap> loadedTileMap = engine.GetMapManager().ParseFromTMJ(filePath);

		if (!loadedTileMap.has_value())
		{
			SDL_Log("Failed to load tile map '%s'.", filePath.string().c_str());
			return false;
		}

		tileMap = std::move(loadedTileMap.value());

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

	void ApplyAnimationComponent(const Json& entityDef, Entity& entity)
	{
		if (!entityDef.contains("animation") || !entityDef["animation"].is_object())
		{
			return;
		}

		const Json& animDef = entityDef["animation"];

		if (!animDef.contains("setName"))
		{
			SDL_Log("Animation component missing 'setName'.");
			return;
		}

		auto& animation = entity.Add<SpriteAnimationComponent>();

		animation.animationSetName = animDef["setName"].get<std::string>();
		animation.currentAnimation = animDef.value("currentAnimation", "idle");
		animation.speedMultiplier = animDef.value("speedMultiplier", 1.0f);
		animation.isPlaying = animDef.value("isPlaying", true);
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

	void ApplyInteractableComponent(const Json& entityDef, Entity& entity)
	{
		if (!entityDef.contains("interactable") || !entityDef["interactable"].is_object())
		{
			return;
		}

		const Json& interactableDef = entityDef["interactable"];
		entity.Add<InteractableComponent>(
			StringToInteractionType(interactableDef.value("interactionType", "default")),
			interactableDef.value("interactionRange", 200.0f),
			interactableDef.value("requiresKey", true),
			interactableDef.value("oneShot", false),
			interactableDef.value("used", false),
			interactableDef.value("dialogueId", "")
		);
	}

	void ApplyControllerComponent(const Json& entityDef, Entity& entity)
	{
		if (!entityDef.contains("controller") || !entityDef["controller"].is_object())
		{
			return;
		}

		const Json& controllerDef = entityDef["controller"];

		if (!controllerDef.contains("path"))
		{
			return;
		}

		const std::string path = controllerDef["path"].get<std::string>();

		std::ifstream file(path);

		if (!file.is_open())
		{
			SDL_Log("Failed to open controller path file: %s", path.c_str());
			return;
		}

		Json controllerJson = Json::parse(file, nullptr, false);

		if (controllerJson.is_discarded())
		{
			SDL_Log("Failed to parse controller JSON file: %s", path.c_str());
			return;
		}

		if (!controllerJson.contains("actions") || !controllerJson["actions"].is_array())
		{
			SDL_Log("Controller JSON is missing 'actions' array or it is not an array.");
			return;
		}

		auto& controller = entity.Add<ControllerComponent>();

		// Loop through actions array
		for (const auto& action : controllerJson["actions"])
		{
			if (!action.contains("action") || !action["action"].is_string())
			{
				continue;
			}

			std::string actionName = action["action"].get<std::string>();

			std::transform(actionName.begin(), actionName.end(), actionName.begin(),
				[](unsigned char c) { return std::tolower(c); });

			if (actionName == "left")
			{
				controller.moveLeftPrimary = StringToScanCode(action.value("primary", ""));
				controller.moveLeftSecondary = StringToScanCode(action.value("secondary", ""));
			}
			else if (actionName == "right")
			{
				controller.moveRightPrimary = StringToScanCode(action.value("primary", ""));
				controller.moveRightSecondary = StringToScanCode(action.value("secondary", ""));
			}
			else if (actionName == "up")
			{
				controller.moveUpPrimary = StringToScanCode(action.value("primary", ""));
				controller.moveUpSecondary = StringToScanCode(action.value("secondary", ""));
			}
			else if (actionName == "down")
			{
				controller.moveDownPrimary = StringToScanCode(action.value("primary", ""));
				controller.moveDownSecondary = StringToScanCode(action.value("secondary", ""));
			}
			else if (actionName == "interact")
			{
				controller.interactPrimary = StringToScanCode(action.value("primary", ""));
				controller.interactSecondary = StringToScanCode(action.value("secondary", ""));
			}
		}
	}

	bool LoadAnimationSetDefinitions(const Json& animDef, Engine& engine)
	{
		if (!animDef.contains("animationSets"))
		{
			return true; // Optional section
		}

		if (!animDef["animationSets"].is_array())
		{
			SDL_Log("Scene file 'animationSets' must be an array.");
			return false;
		}

		for (const Json& animSetDef : animDef["animationSets"])
		{
			if (!animSetDef.contains("name") || !animSetDef.contains("filePath"))
			{
				SDL_Log("AnimationSet definition is missing 'name' or 'filePath'.");
				return false;
			}

			const std::string name = animSetDef["name"].get<std::string>();
			const std::string filePath = animSetDef["filePath"].get<std::string>();

			if (!engine.GetAnimationManager().LoadAnimationSet(filePath))
			{
				SDL_Log("Failed to load animation set from '%s'.", filePath.c_str());
				return false;
			}
		}

		return true;
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
			ApplyInteractableComponent(entityDef, entity);

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

SceneLoadResult SceneLoader::LoadScene(std::string_view filePath, Engine& engine, Registry& registry, TileMap& tileMap)
{
	SceneLoadResult result;
	std::ifstream file(filePath.data());

	if (!file.is_open())
	{
		SDL_Log("Failed to open scene file: %s", filePath.data());
		return result;
	}

	Json document = Json::parse(file, nullptr, false);
	
	if (document.is_discarded())
	{
		SDL_Log("Failed to parse scene JSON file: %s", filePath.data());
		return result;
	}
	
	Json& animDef = document["animationSets"];
		
	if (!animDef.is_object() )
	{
		SDL_Log("Scene file 'animationSets' must be an object.");
		result.loaded = false;

		return result;
	}

	std::string animSetPath = animDef.value("path", "");

	if (!engine.GetAnimationManager().LoadAnimationSet(animSetPath))
	{
		SDL_Log("Failed to load animation set from '%s'.", animSetPath.c_str());
		result.loaded = false;
		return result;
	}

	if (!LoadTextureDefinitions(document, engine, result) ||
		!LoadSoundDefinitions(document, engine, result) ||
		!LoadMusicDefinitions(document, engine, result) ||
		!LoadTileMapDefinitions(document, engine, tileMap) ||
		!LoadEntities(document, registry, result))
	{
		SDL_Log("Failed to load scene from file: %s", filePath.data());
		return result;
	}

	result.loaded = true;
	return result;
}