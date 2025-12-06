#pragma once
#include <functional>
#include <string>
#include <vector>
#include <tmxlite/Map.hpp> 
#include <memory>
#include <unordered_map>
class Assets;

enum class GatewaySide
{
    Top,
    Bottom,
    Left,
    Right
};

struct Gateway
{
    int         targetMapIndex;
    float       posX;
    float       posY;
    GatewaySide side;
};

struct TileInfo
{
    int textureId = -1; // ID tekstury z Assets

    // prostok¹t w tej teksturze (w pikselach)
    uint32_t texX = 0; // lewy górny róg kafelka w teksturze
    uint32_t texY = 0;
    uint32_t texWidth = 0; // zwykle 64
    uint32_t texHeight = 0; // zwykle 64

    bool solid = false; // czy kafelek jest "sztywny" (kolizje)
    bool door  = false; // czy kafelek jest drzwiami
};


class Map
{
public:
    Map() = default;
    bool loadFromTmxFile(const std::string& path, Assets *assets);
    int  getRows() const
    {
        return rows;
    };
    int getColumns() const
    {
        return columns;
    };
    void addGateway(int targetIndex, float posX, float posY)
    {
        gateways_.push_back(Gateway{targetIndex, posX, posY, GatewaySide::Top});
    }
    const std::vector<Gateway>& gateways() const
    {
        return gateways_;
    }
    GatewaySide gatewaySide(int gatewayIndex)
    {
        return gateways_[gatewayIndex].side;
    }
    void setGatewaySide(int gatewayIndex, GatewaySide side)
    {
        if (gatewayIndex >= 0 && gatewayIndex < static_cast<int>(gateways_.size()))
        {
            gateways_[gatewayIndex].side = side;
        }
    }
    void setVisited(bool v)
    {
        visited = v;
    }
    bool isVisited() const
    {
        return visited;
    }

    
    tmx::Map& getTmxMap()
    {
        return mapTmx;
    }
    const TileInfo* getTileInfo(std::uint32_t gid) const
    {
        auto it = gidToTileInfo_.find(gid);
        if (it == gidToTileInfo_.end())
            return nullptr;
        return &it->second;
    }

    const std::vector<tmx::Layer::Ptr>& getLayers() const
    {
        return mapTmx.getLayers();
    }

private:
    bool visited = 0;
    int rows = 0;
    int columns = 0;
    std::vector<Gateway> gateways_;
    tmx::Map mapTmx;
    float mapWidth = 0.f;
    float mapHeight = 0.f;
    std::unordered_map<std::uint32_t, TileInfo> gidToTileInfo_;
};