#include "World.h"
#include "StaticEntity.h"
#include "Player.h"
#include "Assets.h"
#include <algorithm>
#include <utility>
#include <stdexcept>
#include <iostream>
#include "Map.h"


StaticEntity& World::spawnTile(const std::string &texturePath, uint32_t width, uint32_t height, float pos_x, float pos_y, bool solid) {
    const int entityId = assets_ ? assets_->getOrLoadIcon(texturePath) : -1;
    return addEntity<StaticEntity>(entityId, width, height, pos_x ,pos_y, solid);
}

Player& World::spawnPlayer(const std::string &texturePath, uint32_t width, uint32_t height, float pos_x, float pos_y, int maxHp) {
    const int playerId = assets_ ? assets_->getOrLoadIcon(texturePath) : -1;
    Player& p = addEntity<Player>(playerId, width , height , pos_x, pos_y,  maxHp);
    player_ = &p;
    return p;
}

void World::buildFromMap(const std::string &wallTexturePath, const std::string &floorTexturePath, const std::string& doorTexturePath, uint32_t tileW, uint32_t tileH) {
    if (currentMapIndex >= maps_.size() || !maps_[currentMapIndex]) {
        throw std::runtime_error("Invalid map index or map not loaded");
    }
    
    maps_[currentMapIndex]->forEachTile([&](int i, int j, char t) {
        const float x = j * static_cast<float>(tileW);
        const float y = i * static_cast<float>(tileH);
        if (t == '*') {
            spawnTile(wallTexturePath, tileW, tileH, x, y, true);
        } else if (t == '-') {
            spawnTile(floorTexturePath, tileW, tileH, x, y, false);
        } else if (t >= '0' && t <= '9') {
            spawnTile(doorTexturePath, tileW, tileH, x, y, false);
			maps_[currentMapIndex]->addGateway(t - '0', x, y);
        }
    });
}


bool World::intersectsAABB(const Entity &a, const Entity &b) {
    ImVec2 ap = a.getPosition();
    ImVec2 bp = b.getPosition();
    return !(ap.x + a.getWidth() <= bp.x || bp.x + b.getWidth() <= ap.x ||
        ap.y + a.getHeight() <= bp.y || bp.y + b.getHeight() <= ap.y);
}

bool World::intersectsAABBAt(const Entity &a, const Entity &b, float ax, float ay) {
    ImVec2 bp = b.getPosition();
    return !(ax + a.getWidth() <= bp.x || bp.x + b.getWidth() <= ax ||
        ay + a.getHeight() <= bp.y || bp.y + b.getHeight() <= ay);
}

void World::moveWithCollisions(Entity &mover, float dx, float dy, const std::vector<std::unique_ptr<Entity>> &entities) {
    // move along X then Y and resolve against solids
    ImVec2 p = mover.getPosition();
    float newX = p.x + dx;
    float newY = p.y;
    for (const auto& up : entities) {
        if (!up || up.get() == &mover) continue;
        if (!up->isSolid()) continue;
        if (intersectsAABBAt(mover, *up, newX, newY)) {
            if (dx >0) {
                newX = up->getPosition().x - mover.getWidth();
            } else if (dx <0) {
                newX = up->getPosition().x + up->getWidth();
            }
        }
    }

    // move Y
    float newY2 = newY + dy;
    for (const auto& up : entities) {
        if (!up || up.get() == &mover) continue;
        if (!up->isSolid()) continue;
        if (intersectsAABBAt(mover, *up, newX, newY2)) {
            if (dy >0) {
                newY2 = up->getPosition().y - mover.getHeight();
            } else if (dy <0) {
                newY2 = up->getPosition().y + up->getHeight();
            }
        }
    }
    mover.setPosition(newX, newY2);
}

void World::pushOutOfSolids(Entity &mover, const std::vector<std::unique_ptr<Entity>> &entities) {
    ImVec2 p = mover.getPosition();
    for (const auto& up : entities) {
        if (!up || up.get() == &mover) continue;
        if (!up->isSolid()) continue;
     if (!intersectsAABB(mover, *up)) continue;
        // Compute minimal axis separation
        ImVec2 op = up->getPosition();
        float dx1 = (op.x + up->getWidth()) - p.x; // push right
        float dx2 = (p.x + mover.getWidth()) - op.x; // push left
  float dy1 = (op.y + up->getHeight()) - p.y; // push down
      float dy2 = (p.y + mover.getHeight()) - op.y; // push up
  // choose smallest move
        float minX = std::min(dx1, dx2);
  float minY = std::min(dy1, dy2);
    if (minX < minY) {
     // resolve horizontally
   if (dx1 < dx2) p.x = op.x + up->getWidth();
          else p.x = op.x - mover.getWidth();
     } else {
    // resolve vertically
            if (dy1 < dy2) p.y = op.y + up->getHeight();
      else p.y = op.y - mover.getHeight();
        }
    }
    mover.setPosition(p.x, p.y);
}

int World::playerInGateway() {
    if (!player_) return -1;
    
    ImVec2 pos = player_->getPosition();
    float pw = player_->getWidth();
    float ph = player_->getHeight();

  for (auto& g : maps_[currentMapIndex]->gateways()) {
   // Gateway size is 64x64 (same as tiles)
 float gw = 64.0f;
  float gh = 64.0f;
        
        // Require significant overlap - player must be at least 70% inside
  float overlapThreshold = 0.70f;
   float requiredOverlapX = pw * overlapThreshold;
     float requiredOverlapY = ph * overlapThreshold;
        
     // Calculate actual overlap
     float overlapLeft = std::max(0.0f, std::min(pos.x + pw, g.posX + gw) - std::max(pos.x, g.posX));
        float overlapTop = std::max(0.0f, std::min(pos.y + ph, g.posY + gh) - std::max(pos.y, g.posY));
        
        // Check if overlap is sufficient in both dimensions
        if (overlapLeft >= requiredOverlapX && overlapTop >= requiredOverlapY) {
    return g.targetMapIndex;
   }
 }
    return -1;
}

void World::clampToScreen(Entity &mover) {
    if (screenWidth_ <= 0.0f || screenHeight_ <= 0.0f) return;
    
    ImVec2 p = mover.getPosition();

    if (p.x < 0.0f) p.x = 0.0f;
    if (p.y < 0.0f) p.y = 0.0f;
    if (p.x + mover.getWidth() > screenWidth_) p.x = screenWidth_ - mover.getWidth();
    if (p.y + mover.getHeight() > screenHeight_) p.y = screenHeight_ - mover.getHeight();
    
    mover.setPosition(p.x, p.y);
}

void World::update(float dt) {
    for (auto& up : entities_) {
     if (!up) continue;
    up->update(dt);
  
        if (up->isSolid()) continue;
        clampToScreen(*up);
        pushOutOfSolids(*up, entities_);
    }
        this->gatewayIndex = playerInGateway();
        if (gatewayIndex >= 0) {
    this->newScene();
    }
}

void World::newScene() {
    // Validate gatewayIndex before using it
    if (gatewayIndex < 0 || static_cast<size_t>(gatewayIndex) >= maps_.size()) {
   std::cout << "Invalid gateway index: " << gatewayIndex << "\n";
        gatewayIndex = -1;
 return;
    }
    entities_.clear();
    player_ = nullptr;
    
 this->setCurrentMapIndex(gatewayIndex);
    
    this->buildFromMap(
        "assets/design/wall.png",
        "assets/design/floor.png",
      "assets/design/door.png",
        64, 64
    );
    
    // Spawn player at a default location on the new map
    this->spawnPlayer( "assets/characters/hero.png",64, 64, 100.0f, 100.0f, 100 );
    
    std::cout << "Switched to map: " << gatewayIndex << "\n";
    gatewayIndex = -1;
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

void World::addMap(const std::string& path) {
    auto m = std::make_unique<Map>();
    if (!m->loadFromFile(path)) {
        throw std::runtime_error("Could not load map file: " + path);
    }
    maps_.push_back(std::move(m));
}
