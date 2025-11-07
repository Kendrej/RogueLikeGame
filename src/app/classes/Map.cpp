#include "Map.h"
#include "entity.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>


Map::Map(const std::string& path, const std::string& wallPath, const std::string& floorPath, Assets* assets) : mapCharacters(rows, std::vector<char>(columns, ' '))
{
	Map::initializeMapCharacters(path);
	Map::setupMap(wallPath, floorPath, assets);
}

void Map::test() {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			std::cout << mapCharacters[i][j];
		}
		std::cout << std::endl;
	}
}

void Map::initializeMapCharacters(const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "Nie można otworzyć pliku: " << path << std::endl;
		return;
	}
	std::string line;
	for (int i = 0; i < rows && std::getline(file, line); ++i) {
		for (int j = 0; j < columns; ++j) {
			if (j < static_cast<int>(line.size())) {
				mapCharacters[i][j] = line[j];
			}
			else {
				mapCharacters[i][j] = ' '; // brak znaku -> spacja
			}
		}
	}
}

void Map::setupMap(const std::string& wallPath, const std::string& floorPath, Assets* assets)
{
	float posX = 1920.0f / 30.0f;
	float posY = 1080.0f / 17.0f;

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			if (mapCharacters[i][j] == '*') {
				Entity::spawn(mapTiles, assets, wallPath.c_str(), 64, 64, posX * static_cast<float>(j), posY * static_cast<float>(i));
			}
			else if (mapCharacters[i][j] == '-') {
				Entity::spawn(mapTiles, assets, floorPath.c_str(), 64, 64, posX * static_cast<float>(j), posY * static_cast<float>(i));
			}
			else {
				char c = mapCharacters[i][j];
				// Whitespace = puste pole, pomijamy bez bledow
				if (c == ' ' || c == '\t' || c == '\r') {
					continue;
				}
				// Inne znaki: wypisz ostrzezenie i pomin
				std::ostringstream oss;
				oss << "[Map Warning] Nieznany znak w pliku mapy: '" << (c == '\0' ? '?' : c)
					<< "' (0x" << std::hex << static_cast<int>(static_cast<unsigned char>(c)) << std::dec
					<< ") w wierszu " << i << ", kolumnie " << j
					<< ". Dozwolone: '*' (sciana), '-' (podloga), spacja (puste). Pomijam kafelek.";

				std::cerr << oss.str() << std::endl;
				continue;
			}
		}
	}
}

std::vector<std::unique_ptr<Entity>>& Map::getMapTiles()
{
	return mapTiles;
}
