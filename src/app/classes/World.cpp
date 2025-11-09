#include "World.h"
#include "StaticEntity.h"
#include "Player.h"
#include "Assets.h"
#include <algorithm>
#include <utility>
#include "Map.h"


StaticEntity& World::spawnTile(const std::string &texturePath, uint32_t width, uint32_t height, float pos_x, float pos_y, bool solid) {
    const int entityId = assets_ ? assets_->getOrLoadIcon(texturePath) : -1;
    return addEntity<StaticEntity>(entityId, width, height, pos_x ,pos_y, solid);
}

Player& World::spawnPlayer(const std::string &texturePath, uint32_t width, uint32_t height, float pos_x, float pos_y) {
    const int playerId = assets_ ? assets_->getOrLoadIcon(texturePath) : -1;
    Player& p = addEntity<Player>(playerId, width , height , pos_x, pos_y);
    player_ = &p;
    return p;
}

void World::buildFromMap(const Map &map, const std::string &wallTexturePath, const std::string &floorTexturePath, uint32_t tileW, uint32_t tileH) {
    map.forEachTile([&](int i, int j, char t){
        const float x = j * static_cast<float>(tileW);
        const float y = i * static_cast<float>(tileH);
        if (t == '*') {
            spawnTile(wallTexturePath, tileW, tileH, x, y, true);
        } else if (t == '-') {
            spawnTile(floorTexturePath, tileW, tileH, x , y, false);
        }
    });
}


void World::update(float dt) {
    for (auto& up : entities_) {
        if (up) up->update(dt);
    }
}

void World::clear() {
    entities_.clear();
    player_ = nullptr;
}

void World::forEachEntity(const std::function<void(Entity &)> &fn) {
  for ( auto& up: entities_) {
  if (up) fn(*up);
    }
}

bool World::remove(Entity* ptr) {
    if (!ptr) return false;
    auto it = std::remove_if(entities_.begin(), entities_.end(),
        [&](const std::unique_ptr<Entity>& up){ return up.get() == ptr; });
    const bool removed = (it != entities_.end());
    entities_.erase(it, entities_.end());
    if (removed && player_ == ptr) player_ = nullptr;
    return removed;
}

