#pragma once
#include "../ECS/Entity.h"
#include <string>
#include <unordered_map>
#include <vector>

class Engine;
class Registry;
class TileMap;

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
	static SceneLoadResult LoadScene(const std::string& sceneFilePath, Engine& engine, Registry& registry, TileMap& tileMap);
};