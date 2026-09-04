#pragma once
#include "../ECS/Entity.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <expected>

class Engine;
class Scene;

using Json = nlohmann::json;

class SceneLoader final
{
public:	
	static std::expected<void, std::string> LoadScene(const std::filesystem::path& sceneFilePath, Engine& engine, Scene& scene);
	static std::expected<void, std::string> SaveScene(const std::filesystem::path& sceneFilePath, Scene& scene);

private:
	using AssetFilePaths = std::unordered_map<std::string, std::filesystem::path>;

	static bool LoadEntities(const Json& document, Scene& scene);
	static bool LoadTextureDefinitions(const Json& document, Engine& engine, Scene& scene);
	static bool LoadSoundDefinitions(const Json& document, Engine& engine, Scene& scene);
	static bool LoadTileMapDefinitions(const Json& document, Engine& engine, Scene& scene);
	static bool LoadMusicDefinitions(const Json& document, Engine& engine, Scene& scene);
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
	static void ApplyDialogueComponent(const Json& entityDef, Entity& entity);

	static std::string ToAssetPathString(const std::filesystem::path& filePath);
	static std::expected<Json, std::string> SerializeAssetDefinitions(const std::vector<std::string>& assetNames, const AssetFilePaths& filePaths,
																      std::string_view assetType);

	static std::expected<Json, std::string> SerializeEntity(Scene& scene, const Entity& entity);
};