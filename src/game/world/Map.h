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

struct ChestInfo
{
    float posX;
    float posY;
    float width;
    float height;
    bool opened = false;
    bool itemsTaken = false;
    std::vector<std::string> items;
};

struct TileInfo
{
    int textureId = -1; // ID tekstury z Assets
    int currentAnimationState = 0;
    float animationDuration= 0.0f;
    // prostok¹t w tej teksturze (w pikselach)
    uint32_t texX = 0; // lewy górny róg kafelka w teksturze
    uint32_t texY = 0;
    uint32_t texWidth = 0; // zwykle 64
    uint32_t texHeight = 0; // zwykle 64

    bool solid = false; // czy kafelek jest "sztywny" (kolizje)
    bool door  = false; // czy kafelek jest drzwiami
    std::string use = "";    // optional string property read from TMX (e.g., "use")
    bool animated = false;
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
    void addChestInfo(float posX, float posY, float width, float height)
    {
        chestInfo_ = ChestInfo{posX, posY, width, height, false};
    }
    void setChestOpened(bool opened)
    {
        chestInfo_.opened = opened;
    }
    void addItemToChest(const std::string& item)
    {
        chestInfo_.items.push_back(item);
    }
    void setChestItemsTaken(bool taken)
    {
        chestInfo_.itemsTaken = taken;
    }
    ChestInfo getChestInfo() const
    {
        return chestInfo_;
    }
    bool isChestOpened() const
    {
        return chestInfo_.opened;
    }
    bool isChestItemsTaken() const
    {
        return chestInfo_.itemsTaken;
    }
    std::vector<std::string> getChestItems() const
    {
        return chestInfo_.items;
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

    // Animation accessors: returns nullptr if not found
    const std::vector<std::uint32_t>* getAnimatedFrames(std::uint32_t baseGid) const
    {
        auto it = animatedFrames_.find(baseGid);
        if (it == animatedFrames_.end())
            return nullptr;
        return &it->second;
    }
private:
    bool visited = 0;
    int rows = 0;
    int columns = 0;
    std::vector<Gateway> gateways_;
    ChestInfo chestInfo_;
    tmx::Map mapTmx;
    float mapWidth = 0.f;
    float mapHeight = 0.f;
    std::unordered_map<std::uint32_t, TileInfo> gidToTileInfo_;
    std::unordered_map<std::uint32_t, std::vector<std::uint32_t>> animatedFrames_;
};