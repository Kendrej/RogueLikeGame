#include "World.h"
#include "StaticEntity.h"
#include "Player.h"
#include "Assets.h"
#include <algorithm>
#include <utility>
#include <npc/MeleeController.h>
#include <npc/RangeController.h>
#include "LivingEntity.h"
#include <stdexcept>
#include <iostream>
#include "Map.h"
#include "AnimationController.h"
#include "Projectile.h"

StaticEntity& World::spawnTile(const std::string &texturePath, uint32_t width, uint32_t height, float pos_x, float pos_y, bool solid) {
    const int entityId = assets_ ? assets_->getOrLoadIcon(texturePath) : -1;
    return addEntity<StaticEntity>(entityId, width, height, pos_x ,pos_y, solid);
}

Player& World::spawnPlayer(const std::string &texturePath, uint32_t width, uint32_t height, float pos_x, float pos_y, int maxHp) {
    const int playerId = assets_ ? assets_->getOrLoadIcon(texturePath) : -1;
   player_ = std::make_unique<Player>(playerId, width, height, pos_x, pos_y, maxHp);
   Player& p = *player_;
  
   p.setSolid(true);
   p.setAttackDamage(5);
   p.setAttackRange(50.0f);
   p.setAttackCooldown(0.7f);
  
   return *player_;
}

Npc& World::spawnNpc(const std::string &texturePath, uint32_t width, uint32_t height, float pos_x, float pos_y, int maxHp, std::unique_ptr<INpcController> controller, float attackRange) {
    const int npcId = assets_ ? assets_->getOrLoadIcon(texturePath) : -1;
    Npc& n= addEntity<Npc>(npcId, width , height , pos_x, pos_y,  maxHp);
    n.setWorld(this);
    n.setController(std::move(controller));
    n.setSolid(true);
    n.setAttackDamage(5);
    n.setAttackRange(attackRange);
    n.setAttackCooldown(0.7f);
    return n;
}

Projectile& World::spawnProjectile(uint32_t width,uint32_t height,
    float pos_x,float pos_y,ImVec2 velocity,float lifetime,int damage, LivingEntity* owner, const std::string &texturePath)
{
    const int projId =  assets_ ? assets_->getOrLoadIcon(texturePath) : -1;
    auto& projectile = addEntity<Projectile>(
        projId, width, height,
        pos_x, pos_y,
        velocity, lifetime, damage, owner
    );
    return projectile;
}


void World::buildFromMap(const std::string &wallTexturePath, const std::string &floorTexturePath, const std::string& doorTexturePath, uint32_t tileW, uint32_t tileH) {
	gatewayIndexes_.clear();
    if (currentMapIndex >= maps_.size() || !maps_[currentMapIndex]) {
        throw std::runtime_error("Invalid map index or map not loaded");
    }
    
    maps_[currentMapIndex]->forEachTile([&](int i, int j, char t) {
        const float x = j * static_cast<float>(tileW);
        const float y = i * static_cast<float>(tileH) + UI_TOP_BAR_HEIGHT;  // Add UI offset
        if (t == '*') {
            spawnTile(wallTexturePath, tileW, tileH, x, y, true);
        } else if (t == '-'|| t == 'r' || t == 'm') {
            spawnTile(floorTexturePath, tileW, tileH, x, y, false);
        } else if (t >= '0' && t <= '9') {
            // t - '0' is the TARGET map index, not the index in gateways() vector.
            auto & door = spawnTile(doorTexturePath, tileW, tileH, x, y, true);
            const int id = door.getEntityId();
            if (std::find(gatewayIndexes_.begin(), gatewayIndexes_.end(), id) == gatewayIndexes_.end()) {
            gatewayIndexes_.push_back(id);
            }
            maps_[currentMapIndex]->addGateway(t - '0', x, y);
            int newGatewayIndex = static_cast<int>(maps_[currentMapIndex]->gateways().size()) -1; // index of newly added gateway
            maps_[currentMapIndex]->setGatewaySide(newGatewayIndex, getSide(newGatewayIndex));
        }
    });
}

GatewaySide World::getSide(int gatewayIndex) {
    const Gateway& g = maps_[currentMapIndex]->gateways()[gatewayIndex];
    int tileX = static_cast<int>(g.posX / 64.0f);
    int tileY = static_cast<int>((g.posY - UI_TOP_BAR_HEIGHT) / 64.0f); // Account for UI offset
    
    if (tileY <= 1) return GatewaySide::Top;
    if (tileY >= static_cast<int>((screenHeight_ - UI_TOP_BAR_HEIGHT) / 64.0f) - 2) return GatewaySide::Bottom;
    if (tileX <= 1) return GatewaySide::Left;
    if (tileX >= static_cast<int>(screenWidth_ / 64.0f) - 2) return GatewaySide::Right;

    return GatewaySide::Top;
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
            ImVec2 ep = up->getPosition();
            //std::cout << "COLLISION with entity at ("
            //          << ep.x << ", " << ep.y << ")\n";
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

      // Reduced threshold to 30% - allows easy passage through single doors
      // Player (64x64) needs only ~19px overlap (less than 1/3 of player size)
  float overlapThreshold = 0.50f;
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
    if (p.y < UI_TOP_BAR_HEIGHT) p.y = UI_TOP_BAR_HEIGHT;  // Clamp to UI bar bottom
    if (p.x + mover.getWidth() > screenWidth_) p.x = screenWidth_ - mover.getWidth();
    if (p.y + mover.getHeight() > screenHeight_) p.y = screenHeight_ - mover.getHeight();
    
    mover.setPosition(p.x, p.y);
}

void World::update(float dt) {
    std::vector<Entity*> toRemove;

    //player update and animations
    if (player_) {
        player_->update(dt);
        if (auto* animationController = player_->getAnimationController()) {
			ImVec2 vel = player_->getVelocity();
            AnimationType type = animationController->getCurrentAnimationType();

            if (!player_->isAlive()) {
                if(type == AnimationType::WalkLeft || type == AnimationType::IdleLeft || type == AnimationType::DeathLeft)
                    animationController->setCurrentAnimationType(AnimationType::DeathLeft);
                else{
                    animationController->setCurrentAnimationType(AnimationType::DeathRight);
                }
                animationController->update(dt);
				return;
            }
            else if (player_->isDamaged()) {
				player_->setDamaged(false);
                if(type == AnimationType::WalkLeft || type == AnimationType::IdleLeft)
                    animationController->setCurrentAnimationType(AnimationType::HurtLeft);
                else{
                    animationController->setCurrentAnimationType(AnimationType::HurtRight);
                }
            }
            else if (type != AnimationType::HurtRight && type != AnimationType::HurtLeft){
                if (vel.x != 0.0f || vel.y != 0.0f) {
                    if (vel.x < 0.0f)
                        animationController->setCurrentAnimationType(AnimationType::WalkLeft);
                    else if (vel.x > 0.0f) {
                        animationController->setCurrentAnimationType(AnimationType::WalkRight);
                    }
                }
                else {
                    if (type == AnimationType::WalkLeft || type == AnimationType::IdleLeft)
                        animationController->setCurrentAnimationType(AnimationType::IdleLeft);
                    else {
                        animationController->setCurrentAnimationType(AnimationType::IdleRight);
                    }
                }
            }
			animationController->update(dt);
        }
    }

    //update all the entities
    for (auto& up : entities_) {
        if (!up) continue;
        up->update(dt);
        if(auto* livingEntity = dynamic_cast<LivingEntity*>(up.get())) {
            if (auto* animationController = livingEntity->getAnimationController()) {
                ImVec2 vel = livingEntity->getVelocity();
                AnimationType type = animationController->getCurrentAnimationType();

                if (!livingEntity->isAlive()){
					livingEntity->setSolid(false);
                    if (type == AnimationType::WalkLeft || type == AnimationType::IdleLeft || type == AnimationType::DeathLeft)
                        animationController->setCurrentAnimationType(AnimationType::DeathLeft);
                    else {
                        animationController->setCurrentAnimationType(AnimationType::DeathRight);
                    }
                    animationController->update(dt);
                    continue;
                }
                else if (livingEntity->isDamaged()) {
                    livingEntity->setDamaged(false);
                    if (type == AnimationType::WalkLeft || type == AnimationType::IdleLeft)
                        animationController->setCurrentAnimationType(AnimationType::HurtLeft);
                    else {
                        animationController->setCurrentAnimationType(AnimationType::HurtRight);
                    }
                }
                else if (type != AnimationType::HurtRight && type != AnimationType::HurtLeft) {
                    if (vel.x != 0.0f || vel.y != 0.0f) {
                        if (vel.x < 0.0f)
                            animationController->setCurrentAnimationType(AnimationType::WalkLeft);
                        else if (vel.x > 0.0f) {
                            animationController->setCurrentAnimationType(AnimationType::WalkRight);
                        }
                    }
                    else {
                        if (type == AnimationType::WalkLeft || type == AnimationType::IdleLeft)
                            animationController->setCurrentAnimationType(AnimationType::IdleLeft);
                        else {
                            animationController->setCurrentAnimationType(AnimationType::IdleRight);
                        }
                    }
                }
                animationController->update(dt);
            }
        }
    }

    //projectile
    for (auto &up : entities_) {
        auto* proj = dynamic_cast<Projectile*>(up.get());
        if (!proj) continue;

        if (proj->isDead()) {
            toRemove.push_back(proj);
            continue;
        }

        //projectile collision with solid tiles / solid entities / livingEntity
        for (auto& other: entities_) {
            if ( !other || other.get() == proj) continue;
            if (!other->isSolid()) continue;
            // Ignore collision with projectile owner to avoid immediate self-collision on spawn
            if (other.get() == proj->getOwner()) continue;

            if (intersectsAABB(*proj, *other)) {
                if (auto* living = dynamic_cast<LivingEntity*>(other.get())) {
                    if (living != proj -> getOwner()) {
                        living->takeDamage(proj->getDamage());
                    }
                }
                proj->kill();
                toRemove.push_back(proj);
                break;
            }
        }
        if (proj->isDead()) continue;


        //projectile collision with player
        if ( player_ ) {
            auto* livingPlayer = dynamic_cast<LivingEntity*>(getPlayer());
            if (livingPlayer &&
                livingPlayer != proj->getOwner() &&
                intersectsAABB(*proj, *livingPlayer))
            {
                livingPlayer->takeDamage(proj->getDamage());
                proj->kill();
                toRemove.push_back(proj);
            }
        }
    }
//player movement
    if (player_)
    {
        auto *livingPlayer = dynamic_cast<LivingEntity*>(player_.get());
        if (livingPlayer) {
        ImVec2 vel = livingPlayer->getVelocity();
            float dx = vel.x * dt;
            float dy = vel.y * dt;
            moveWithCollisions(*livingPlayer, dx, dy, entities_);
        }
        clampToScreen(*player_);
    }
    //npc movement

    for (auto &up : entities_) {
        if (!up) continue;
        auto *living = dynamic_cast<LivingEntity*>(up.get());
        if (!living) continue;
        if (!living->isAlive()) continue;

        ImVec2 vel = living->getVelocity();
        float dx = vel.x * dt;
        float dy = vel.y * dt;
        moveWithCollisions(*living, dx, dy, entities_);
        clampToScreen(*living);
    }

    // Remove dead projectiles
    for (auto* e : toRemove) {
        remove(e);
    }

	int aliveNpcCount = 0;
    for(auto& up : entities_) {
		if (!up) continue;
        auto* npc = dynamic_cast<Npc*>(up.get());
        if (npc && npc->isAlive()) {
            aliveNpcCount++;
        }
	}
    if (aliveNpcCount ==0 && !doorsUnlocked_) {
        // Unlock every entity whose entityId matches any collected door id
            for (int doorEntityId : gatewayIndexes_) {
                for (auto &up : entities_) {
                    if (!up) continue;
                        if (up->getEntityId() == doorEntityId) {
                        up->setSolid(false);
                        }
                    }
            }
        doorsUnlocked_ = true;
	}
    

    // doors-teleports
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
    
    // Save the gateway side from the current map BEFORE switching
    GatewaySide entrySide = GatewaySide::Top;
    float gatewayX = 0.0f;
    float gatewayY = 0.0f;
    
    for (const auto& g : maps_[currentMapIndex]->gateways()) {
        if (g.targetMapIndex == gatewayIndex) {
            int gwIndex = static_cast<int>(&g - &maps_[currentMapIndex]->gateways()[0]);
            entrySide = maps_[currentMapIndex]->gatewaySide(gwIndex);
            gatewayX = g.posX;
            gatewayY = g.posY;
            break;
        }
    }
    
    entities_.clear();
  
    this->setCurrentMapIndex(gatewayIndex);
    doorsUnlocked_ = false;

    this->buildFromMap(
        "assets/design/wall.png",
        "assets/design/floor.png",
        "assets/design/door.png",
        64, 64
    );
    if (!maps_[currentMapIndex]->isVisited()) {
        spawnNpcs();  
    }

    // Move player to new position (player attributes are preserved!)
    if (player_) {
        spawnPlayerInNewScene(entrySide, gatewayX, gatewayY);
    }
    
    //std::cout << "Switched to map: " << gatewayIndex << "\n";
    gatewayIndex = -1;
}

void World::spawnNpcs() {
    maps_[currentMapIndex]->forEachTile([&](int i, int j, char t) {
        const float x = j * static_cast<float>(64);
        const float y = i * static_cast<float>(64) + UI_TOP_BAR_HEIGHT;  // Add UI offset
        if (t == 'm') {
            auto& npc = this->spawnNpc("assets/characters/angel.png",
                64, 64,
                x, y + World::UI_TOP_BAR_HEIGHT,  // Add UI offset to NPC spawn
                10, std::make_unique<MeleeController>(), 70.0f);
            npc.createAnimationController(assets_, 100,
                "assets/animations/Orc-Walk-right.png", 8, "assets/animations/Orc-Walk-left.png", 8,
                "assets/animations/Orc-Idle-right.png", 6, "assets/animations/Orc-Idle-left.png", 6,
                "assets/animations/Orc-Hurt-right.png", 4, "assets/animations/Orc-Hurt-left.png", 4,
                "assets/animations/Orc-Death-right.png", 4, "assets/animations/Orc-Death-left.png", 4);
        }
        else if (t == 'r') {
            auto& npc = this->spawnNpc("assets/characters/hero.png",
                64, 64,
                x, y + World::UI_TOP_BAR_HEIGHT,  // Add UI offset to NPC spawn
                10, std::make_unique<RangeController>(), 300.0f);
            npc.createAnimationController(assets_, 100,
                "assets/animations/SkeletonArcher-Walk-right.png", 8, "assets/animations/SkeletonArcher-Walk-left.png", 8,
                "assets/animations/SkeletonArcher-Idle-right.png", 6, "assets/animations/SkeletonArcher-Idle-left.png", 6,
                "assets/animations/SkeletonArcher-Hurt-right.png", 4, "assets/animations/SkeletonArcher-Hurt-left.png", 4,
                "assets/animations/SkeletonArcher-Death-right.png", 4, "assets/animations/SkeletonArcher-Death-left.png", 4);
        }
    });
    maps_[currentMapIndex]->setVisited(true);
}

void World::spawnPlayerInNewScene(GatewaySide entrySide, float sourceGatewayX, float sourceGatewayY) {
    constexpr float tile = 64.0f;
     //std::cout << screenHeight_;
    switch (entrySide) {
        case GatewaySide::Top:
            // Entering from top, exit at bottom - spawn as close to bottom edge as possible
			sourceGatewayX = player_->getPosition().x; 
            player_->setPosition(sourceGatewayX, screenHeight_ - tile - tile); //1080 - sciana - gracz
            break;
        case GatewaySide::Bottom:
            // Entering from bottom, exit at top - spawn as close to top edge as possible (after UI bar)
            sourceGatewayX = player_->getPosition().x;
            player_->setPosition(sourceGatewayX, UI_TOP_BAR_HEIGHT +   tile ); // sciana + pasek
            break;
        case GatewaySide::Left:
            // Entering from left, exit at right - spawn as close to right edge as possible
            sourceGatewayY = player_->getPosition().y;
            player_->setPosition(screenWidth_ - tile- tile, sourceGatewayY);
            break;
        case GatewaySide::Right:
            // Entering from right, exit at left - spawn as close to left edge as possible
            sourceGatewayY = player_->getPosition().y;
            player_->setPosition(tile, sourceGatewayY);
            break;
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
    
    if (player_.get() == ptr) {
        player_.reset();
        return true;
    }
    
    auto it = std::remove_if(entities_.begin(), entities_.end(),
    [&](const std::unique_ptr<Entity>& up){ return up.get() == ptr; });
    const bool removed = (it != entities_.end());
    entities_.erase(it, entities_.end());
    return removed;
}

void World::addMap(const std::string& path) {
    auto m = std::make_unique<Map>();
    if (!m->loadFromFile(path)) {
        throw std::runtime_error("Could not load map file: " + path);
    }
    maps_.push_back(std::move(m));
}
