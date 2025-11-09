#pragma once
#include <string>
#include <vector>
#include <functional>

class Assets;

class Map {
public:
    Map() = default;
    bool loadFromFile(const std::string& path);
    int getRows() const { return rows; };
    int getColumns() const {return columns; };
    char tileAt(int r, int c) const;
    void forEachTile(const std::function<void(int,int,char)>& fn) const;
private:
    int rows = 0;
    int columns = 0;
    std::vector<std::vector<char>> grid;
};