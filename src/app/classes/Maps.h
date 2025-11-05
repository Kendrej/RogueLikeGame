#pragma once

#include "entity.h"

#include <memory>
#include <string>
#include <vector>

class Entity;
class Assets;

class Map {
public:
	Map(const std::string& path, const std::string& wallPath, const std::string& floorPath, Assets* assets);
	void test();
	void initializeMapCharacters(const std::string& path);
	void setupMap(const std::string& wallPath, const std::string& floorPath, Assets* assets);
	std::vector<std::unique_ptr<Entity>>& getMapTiles();
private:
	std::vector<std::unique_ptr<Entity>> mapTiles;
	const int rows = 17;
	int columns = 30;
	std::vector<std::vector<char>> mapCharacters;
};