#pragma once
#include <string>
#include <vector>
#include <functional>

class Assets;

struct Gateway {
    int targetMapIndex;
    float posX;
	float posY;
};

class Map {
public:
    Map() = default;
    bool loadFromFile(const std::string& path);
    int getRows() const { return rows; };
    int getColumns() const {return columns; };
    char tileAt(int r, int c) const;
    void forEachTile(const std::function<void(int,int,char)>& fn) const;
    void addGateway(int targetIndex, float posX, float posY) {
        gateways_.push_back(Gateway{ targetIndex, posX, posY });
    }
    const std::vector<Gateway>& gateways() const { return gateways_; }
private:
    int rows = 0;
    int columns = 0;
    std::vector<std::vector<char>> grid;
	std::vector<Gateway> gateways_;
};