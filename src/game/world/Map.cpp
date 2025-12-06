#include "Map.h"

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include "engine/gfx/Assets.h"

bool Map::loadFromTmxFile(const std::string& path, Assets *assets)
{
    if (!mapTmx.load(path))
    {
        return false;
    }

    gidToTileInfo_.clear();

    auto tileCount = mapTmx.getTileCount();
    auto tileSize  = mapTmx.getTileSize();
    mapWidth  = static_cast<float>(mapTmx.getTileCount().x * mapTmx.getTileSize().x);
    mapHeight = static_cast<float>(mapTmx.getTileCount().y * mapTmx.getTileSize().y);

    rows = tileCount.y;
    columns = tileCount.x;

    const auto& tilesets = mapTmx.getTilesets();

    for (const auto& ts : tilesets)
    {
        std::uint32_t firstGID = ts.getFirstGID();
        tileSize = ts.getTileSize(); // 64x64

        const auto& tiles = ts.getTiles(); // <tile id="..."><image .../></tile>

        for (const auto& tile : tiles)
        {
            std::uint32_t localId = tile.ID;            // 0, 1, 2, 3...
            std::uint32_t gid     = firstGID + localId; // globalny ID w warstwach

            std::string fullPath = tile.imagePath; // np. "./assets/../designs/floor.png"

            int textureId = assets->getOrLoadIcon(fullPath); // tu Twój Assets
            if (textureId < 0)
            {
                std::cerr << "Failed to load tile image: " << fullPath << "\n";
                continue;
            }

            TileInfo info;
            info.textureId = textureId;
            info.texX = 0; // cały obrazek
            info.texY = 0;
            info.texWidth = tileSize.x; // 64
            info.texHeight = tileSize.y; // 64
            info.solid = false;
            for (const auto& prop : tile.properties)
            {
                if (prop.getName() == "solid" && prop.getBoolValue())
                {
                    info.solid = true;
                }
                else if (prop.getName() == "door" && prop.getBoolValue())
                {
                    info.door = true;
                }
            }
            gidToTileInfo_[gid] = info;
        }
    }
    return true;
}
