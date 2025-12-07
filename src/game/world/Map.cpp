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
        uint32_t    tileW      = tileSize.x;
        uint32_t    tileH      = tileSize.y;

        const auto& imagePath = ts.getImagePath();
        const auto& tiles     = ts.getTiles(); // <tile id="..."><image .../></tile>

        if (imagePath.empty())
        {
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
                info.texX      = 0; // cały obrazek
                info.texY      = 0;
                info.texWidth  = tileSize.x; // 64
                info.texHeight = tileSize.y; // 64
                info.solid     = false;
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
        else
        {
            int textureId = assets->getOrLoadIcon(imagePath); // tu Twój Assets
            if (textureId < 0)
            {
                std::cerr << "Failed to load tile image: " << imagePath << "\n";
                continue;
            }
            uint32_t tileCountIntileset = ts.getTileCount();
            uint32_t columnsInTileset = ts.getColumnCount();
            const uint32_t spacing = 0;
            const uint32_t margin = 0;

            if (columnsInTileset == 0)
            {
                columnsInTileset = tileCountIntileset;
            }

            for (std::uint32_t localId = 0; localId < tileCountIntileset; ++localId)
            {
                std::uint32_t gid = firstGID + localId;

                uint32_t col = localId % columnsInTileset;
                uint32_t row = localId / columnsInTileset;

                uint32_t texX = margin + col * (tileW + spacing);
                uint32_t texY = margin + row * (tileH + spacing);

                TileInfo info{};
                info.textureId = textureId;
                info.texX      = texX;
                info.texY      = texY;
                info.texWidth  = tileW;
                info.texHeight = tileH;
                info.solid     = false;
                info.door      = false;

                const auto& tiles = ts.getTiles(); // vector<Tile>
                auto it = std::find_if(tiles.begin(), tiles.end(), [&](const auto& t) { return t.ID == localId; });

                if (it != tiles.end())
                {
                    const auto& tileWithProps = *it;
                    for (const auto& prop : tileWithProps.properties)
                    {
                        if (prop.getName() == "solid" && prop.getBoolValue())
                            info.solid = true;
                        if (prop.getName() == "door" && prop.getBoolValue())
                            info.door = true;
                    }
                }
                gidToTileInfo_[gid] = info;
            }
        }
    }
    return true;
}
