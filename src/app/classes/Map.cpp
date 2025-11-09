#include "Map.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>


bool Map::loadFromFile(const std::string &path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "Unable to open map file: " << path << "\n";
        rows = columns = 0;
        grid.clear();
        return false;
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(f, line)) {
        lines.push_back(line);
    }
    rows = static_cast<int>(lines.size());
    columns = 0;
    for (const auto& L: lines) {
        columns = std::max(columns, static_cast<int>(L.size()));
    }
    grid.assign(rows, std::vector<char>(columns, ' '));
    for (int i = 0; i < rows; i++) {
        const auto& L = lines[i];
        for (int j = 0; j < static_cast<int>(L.size()); j++) {
            grid[i][j] = L[j];
        }
    }
    return true;
}


char Map::tileAt(int r, int c) const {
    if  (r < 0 || c < 0 || r >= rows || c >= columns) return ' ';
    return grid[r][c];
}

void Map::forEachTile(const std::function<void(int,int,char)>& fn) const {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            fn(i, j, grid[i][j]);
        }
    }
}



