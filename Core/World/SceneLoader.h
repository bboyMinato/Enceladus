#pragma once
#include "../ECS/Entity.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>

class Engine;
class Registry;
class TileMap;

using Json = nlohmann::json;

struct SceneLoadResult
{
	bool loaded{ false };	
	
	std::unordered_map<std::string, Entity> entities;

	std::vector<std::string> loadedTextures;
	std::vector<std::string> loadedSounds;
	std::vector<std::string> loadedMusic;

	std::string autoPlayedSound;
	std::string autoPlayedMusic;
};

class SceneLoader final
{
public:
	static SceneLoadResult LoadScene(std::string_view sceneFilePath, Engine& engine, Registry& registry, TileMap& tileMap);

private:
	static bool LoadEntities(const Json& document, Registry& registry, SceneLoadResult& result);
	static bool LoadTextureDefinitions(const Json& document, Engine& engine, SceneLoadResult& result);
	static bool LoadSoundDefinitions(const Json& document, Engine& engine, SceneLoadResult& result);
	static bool LoadTileMapDefinitions(const Json& document, Engine& engine, TileMap& tileMap);
	static bool LoadMusicDefinitions(const Json& document, Engine& engine, SceneLoadResult& result);
	static bool LoadAnimationSetDefinitions(const Json& animDef, Engine& engine);
	
	static void ApplyControllerComponent(const Json& entityDef, Entity& entity);
	static void ApplyInteractableComponent(const Json& entityDef, Entity& entity);
	static void ApplyMovementComponent(const Json& entityDef, Entity& entity);
	static void ApplyCameraComponent(const Json& entityDef, Entity& entity);
	static void ApplyColliderComponent(const Json& entityDef, Entity& entity);
	static void ApplyTagComponent(const Json& entityDef, Entity& entity);
	static void ApplyAnimationComponent(const Json& entityDef, Entity& entity);
	static void ApplySpriteComponent(const Json& entityDef, Entity& entity);
	static void ApplyTransformComponent(const Json& entityDef, Entity& entity);
};