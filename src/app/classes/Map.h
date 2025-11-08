#pragma once

#include "entity.h"

#include <memory>
#include <string>
#include <vector>

class Assets;

class Map {
public:
	Map(const std::string& path, const std::string& wallPath, const std::string& floorPath, Assets* assets);
	void test();
	void initializeMapCharacters(const std::string& path);
	void setupMap(const std::string& wallPath, const std::string& floorPath, Assets* assets);
	std::vector<std::unique_ptr<Entity>>& getMapTiles();
private:
	void spawnMapTile(const std::string& texturePath, uint32_t width, uint32_t height, float posX, float posY, Assets* assets);
	std::vector<std::unique_ptr<Entity>> mapTiles;
	const int rows = 17;
	int columns = 30;
	std::vector<std::vector<char>> mapCharacters;
};