#include "World.h"

#include "game/entities/animation/AnimationController.h"
#include "engine/gfx/Assets.h"
#include "game/entities/LivingEntity.h"
#include "../factory/PlayerFactory.h"
#include "game/world/Map.h"
#include "utils/MathUtils.h"
#include "game/entities/Player.h"
#include "game/entities/Projectile.h"
#include "../factory/NpcFactory.h"
#include "game/item/consumable/HealthPotion.h"
#include "game/item/consumable/SpeedPotion.h"
#include "game/item/consumable/StrengthPotion.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <tmxlite/TileLayer.hpp>

Entity& World::spawnTile(const std::string& texturePath, uint32_t width, uint32_t height, float pos_x,
                               float pos_y, bool solid)
{
    const int entityId = assets_ ? assets_->getOrLoadIcon(texturePath) : -1;
    return addEntity<Entity>(entityId, width, height, pos_x, pos_y, solid);
}

Player& World::spawnPlayer(ImVec2 pos)
{
    player_ = std::unique_ptr<Player>(PlayerFactory::createPlayer(*this, pos));
    return *player_;
}

Npc& World::spawnNpc(NpcType type, ImVec2 pos)
{
    Npc* npc = NpcFactory::createNpc(type, *this, pos);
    entities_.emplace_back(npc);
    return *npc;
}

Projectile& World::spawnProjectile(uint32_t width, uint32_t height, float pos_x, float pos_y, ImVec2 velocity,
                                   float lifetime, int damage, LivingEntity* owner, const std::string& texturePath)
{
    const int projId = assets_ ? assets_->getOrLoadIcon(texturePath) : -1;
    auto& projectile = addEntity<Projectile>(projId, width, height, pos_x, pos_y, velocity, lifetime, damage, owner);
    return projectile;
}

void World::performMeleeAttack(LivingEntity& attacker)
{
    const float range  = attacker.getMeleeRange();
    const int   damage = attacker.getMeleeDamage();

    // brak zasięgu albo dmg = brak ataku
    if (range <= 0.0f || damage <= 0)
    {
        return;
    }

    ImVec2 pos = attacker.getPosition();
    float  w   = static_cast<float>(attacker.getWidth());
    float  h   = static_cast<float>(attacker.getHeight());

    // Kierunek ataku oparty głównie na facingDir (ostatni kierunek ruchu / patrzenia).
    ImVec2 dir = attacker.getFacingDir();
    attacker.setAimLock(0.5f);
    if (dir.x == 0.0f && dir.y == 0.0f)
    {
        dir = ImVec2{0.0f, 1.0f};
    }


    float attackX = 0.0f;
    float attackY = 0.0f;
    float attackW = 0.0f;
    float attackH = 0.0f;

    if (std::abs(dir.x) >= std::abs(dir.y))
    {
        // poziomo
        if (dir.x >= 0.0f)
        {
            // w prawo
            attackX = pos.x + w;
            attackY = pos.y;
            attackW = range;
            attackH = h;
        }
        else
        {
            // w lewo
            attackX = pos.x - range;
            attackY = pos.y;
            attackW = range;
            attackH = h;
        }
    }
    else
    {
        // pionowo
        if (dir.y >= 0.0f)
        {
            // w dół
            attackX = pos.x;
            attackY = pos.y + h;
            attackW = w;
            attackH = range;
        }
        else
        {
            // w górę
            attackX = pos.x;
            attackY = pos.y - range;
            attackW = w;
            attackH = range;
        }
    }

    // --- Trafienia w encje w entities_ (NPC itd.) ---
    for (auto& up : entities_)
    {
        if (!up)
            continue;

        auto* target = dynamic_cast<LivingEntity*>(up.get());
        if (!target)
            continue;
        if (target == &attacker)
            continue;
        if (!target->isAlive())
            continue;

        if (intersectsRectWithEntity(*target, attackX, attackY, attackW, attackH))
        {
            target->takeDamage(damage);
            target->setDamaged(true); // możesz wykorzystać do animacji 'Hurt'
        }
    }

    // --- Trafienie w playera (jeśli attacker to NPC, a player_ nie jest w entities_) ---
    if (player_)
    {
        auto* playerLiving = dynamic_cast<LivingEntity*>(player_.get());
        if (playerLiving && playerLiving != &attacker && playerLiving->isAlive())
        {
            if (intersectsRectWithEntity(*playerLiving, attackX, attackY, attackW, attackH))
            {
                playerLiving->takeDamage(damage);
                playerLiving->setDamaged(true);
            }
        }
    }
}

void World::performRangedAttack(LivingEntity& attacker, ImVec2 direction)
{
    const std::string projTexture  = "assets/design/Arrow01.png";
    const uint32_t    projW        = 32;
    const uint32_t    projH        = 32;
    const float       projSpeed    = 600.0f;
    const float       projLifetime = 3.0f;

    ImVec2 pos = attacker.getPosition();
    ImVec2 attackerCenter{pos.x + 0.5f * attacker.getWidth(), pos.y + 0.5f * attacker.getHeight()};
    ImVec2 normDir = normalize(direction);
    float spawnDistance = (attacker.getWidth() * 0.5f) + (projW * 0.5f) + 5.0f;
    float spawnCenterX = attackerCenter.x + (normDir.x * spawnDistance);
    float spawnCenterY = attackerCenter.y + (normDir.y * spawnDistance);

    const float projHalfW = static_cast<float>(projW) * 0.5f;
    const float projHalfH = static_cast<float>(projH) * 0.5f;
    // const float offsetX   = (static_cast<float>(attacker.getWidth()) * 0.5f) + projHalfW;

    ImVec2 spawnPos{spawnCenterX - projHalfW , spawnCenterY - projHalfH};

    ImVec2 velocity{normDir.x * projSpeed, normDir.y * projSpeed};

    spawnProjectile(projW, projH, spawnPos.x, spawnPos.y, velocity, projLifetime, attacker.getRangedDamage(), &attacker,
                    projTexture);
    attacker.setFacingDir(normDir);
    attacker.setAimLock(0.5f);

}

void World::buildFromTmxMap() {
    doorEntities_.clear();
    if (currentMapIndex >= maps_.size() || !maps_[currentMapIndex])
    {
        throw std::runtime_error("Invalid map index or map not loaded");
    }

    Map& map = *maps_[currentMapIndex];
    const auto& tmxMap = map.getTmxMap();
    const auto& layers = map.getLayers();
    const auto tileSize = tmxMap.getTileSize(); // np. 64x64

    for (const auto& layer : layers)
    {
        if (layer->getType() == tmx::Layer::Type::Tile)
        {

            const auto* tileLayer = dynamic_cast<const tmx::TileLayer*>(layer.get());
            if (!tileLayer)
                continue;

            const auto& tiles     = tileLayer->getTiles();
            const auto  layerSize = tileLayer->getSize();

            for (std::size_t y = 0; y < layerSize.y; ++y)
            {
                for (std::size_t x = 0; x < layerSize.x; ++x)
                {
                    std::size_t   index = x + y * layerSize.x;
                    const auto& tileData = tiles[index];

                    if (tileData.ID == 0)
                        continue; // puste pole


                    bool flipH = (tileData.flipFlags & tmx::TileLayer::Horizontal) != 0;
                    bool flipV = (tileData.flipFlags & tmx::TileLayer::Vertical) != 0;
                    bool flipD = (tileData.flipFlags & tmx::TileLayer::Diagonal) != 0;

                    std::uint32_t gid = tileData.ID;

                    const TileInfo* info = map.getTileInfo(gid);
                    if (!info)
                        continue; // brak powiązania GID -> tekstura (błąd w tilesecie / loadFromTmxFile)

                    const float posX = static_cast<float>(x * tileSize.x);
                    const float posY = static_cast<float>(y * tileSize.y) + UI_TOP_BAR_HEIGHT;
                    auto& tile = addEntity<Entity>(info->textureId, info->texWidth, info->texHeight, posX, posY,
                                                         info->solid);
                    tile.setTexOffset(info->texX, info->texY);
                    tile.setFlipFlags(flipH, flipV, flipD);
                    if (info->door)
                    {
                            doorEntities_.push_back(&tile);
                    }
                }
            }
        }
        else if (layer->getType() == tmx::Layer::Type::Object){
            const auto* objGroup = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
            if (!objGroup)
                continue;

            for (const auto& obj : objGroup->getObjects())
            {
                int target = -1;

                // czytamy property "target" z obiektu
                for (const auto& prop : obj.getProperties())
                {
                    if (prop.getName() == "target")
                    {
                        target = prop.getIntValue();
                        break;
                    }
                }

                if (target < 0)
                    continue; // obiekt nie jest teleportem

                // Pozycja obiektu w układzie mapy (Tiled: bez UI offsetu)
                float posX = obj.getPosition().x;
                float posY = obj.getPosition().y + UI_TOP_BAR_HEIGHT; // dostosowanie do świata gry

                // Zapisujemy gateway w Map (tak jak wcześniej przy '0'..'9')
                map.addGateway(target, posX, posY);

                int newGwIdx = static_cast<int>(map.gateways().size()) - 1;

                // Ustalamy stronę (Top/Bottom/Left/Right) na podstawie pozycji
                map.setGatewaySide(newGwIdx, getSide(newGwIdx));

            }
        }
    }
   
}

GatewaySide World::getSide(int gwIdx)
{
    const Gateway& g     = maps_[currentMapIndex]->gateways()[gwIdx];
    int            tileX = static_cast<int>(g.posX / 64.0f);
    int            tileY = static_cast<int>((g.posY - UI_TOP_BAR_HEIGHT) / 64.0f); // Account for UI offset

    int mapRows = maps_[currentMapIndex]->getRows();
    int mapCols = maps_[currentMapIndex]->getColumns();

    if (tileY <= 2)
        return GatewaySide::Top;
    if (tileY >= mapRows - 3)
        return GatewaySide::Bottom;
    if (tileX <= 2)
        return GatewaySide::Left;
    if (tileX >= mapCols - 3)
        return GatewaySide::Right;

    return GatewaySide::Top;
}

bool World::intersectsAABB(const Entity& a, const Entity& b)
{
    ImVec2 ap = a.getPosition();
    ImVec2 bp = b.getPosition();
    return !(ap.x + a.getWidth() <= bp.x || bp.x + b.getWidth() <= ap.x || ap.y + a.getHeight() <= bp.y ||
             bp.y + b.getHeight() <= ap.y);
}

bool World::intersectsAABBAt(const Entity& a, const Entity& b, float ax, float ay)
{
    ImVec2 bp = b.getPosition();
    return !(ax + a.getWidth() <= bp.x || bp.x + b.getWidth() <= ax || ay + a.getHeight() <= bp.y ||
             bp.y + b.getHeight() <= ay);
}

bool World::intersectsRectWithEntity(const Entity& e, float rx, float ry, float rw, float rh)
{
    ImVec2 p  = e.getPosition();
    float  eh = static_cast<float>(e.getHeight());
    float  ew = static_cast<float>(e.getWidth());

    return !(p.x + ew <= rx || rx + rw <= p.x || p.y + eh <= ry || ry + rh <= p.y);
}

void World::moveWithCollisions(Entity& mover, float dx, float dy, const std::vector<std::unique_ptr<Entity>>& entities)
{
    // move along X then Y and resolve against solids
    ImVec2 p    = mover.getPosition();
    float  newX = p.x + dx;
    float  newY = p.y;
    for (const auto& up : entities)
    {
        if (!up || up.get() == &mover)
            continue;
        if (!up->isSolid())
            continue;
        if (intersectsAABBAt(mover, *up, newX, newY))
        {
            ImVec2 ep = up->getPosition();
            // std::cout << "COLLISION with entity at ("
            //           << ep.x << ", " << ep.y << ")\n";
            if (dx > 0)
            {
                newX = up->getPosition().x - mover.getWidth();
            }
            else if (dx < 0)
            {
                newX = up->getPosition().x + up->getWidth();
            }
        }
    }

    // move Y
    float newY2 = newY + dy;
    for (const auto& up : entities)
    {
        if (!up || up.get() == &mover)
            continue;
        if (!up->isSolid())
            continue;
        if (intersectsAABBAt(mover, *up, newX, newY2))
        {
            if (dy > 0)
            {
                newY2 = up->getPosition().y - mover.getHeight();
            }
            else if (dy < 0)
            {
                newY2 = up->getPosition().y + up->getHeight();
            }
        }
    }
    mover.setPosition(newX, newY2);
}

void World::pushOutOfSolids(Entity& mover, const std::vector<std::unique_ptr<Entity>>& entities)
{
    ImVec2 p = mover.getPosition();
    for (const auto& up : entities)
    {
        if (!up || up.get() == &mover)
            continue;
        if (!up->isSolid())
            continue;
        if (!intersectsAABB(mover, *up))
            continue;
        // Compute minimal axis separation
        ImVec2 op  = up->getPosition();
        float  dx1 = (op.x + up->getWidth()) - p.x;    // push right
        float  dx2 = (p.x + mover.getWidth()) - op.x;  // push left
        float  dy1 = (op.y + up->getHeight()) - p.y;   // push down
        float  dy2 = (p.y + mover.getHeight()) - op.y; // push up
        // choose smallest move
        float minX = std::min(dx1, dx2);
        float minY = std::min(dy1, dy2);
        if (minX < minY)
        {
            // resolve horizontally
            if (dx1 < dx2)
                p.x = op.x + up->getWidth();
            else
                p.x = op.x - mover.getWidth();
        }
        else
        {
            // resolve vertically
            if (dy1 < dy2)
                p.y = op.y + up->getHeight();
            else
                p.y = op.y - mover.getHeight();
        }
    }
    mover.setPosition(p.x, p.y);
}

int World::playerInGateway()
{
    if (!player_)
        return -1;

    ImVec2 pos = player_->getPosition();
    float  pw  = static_cast<float>(player_->getWidth());
    float  ph  = static_cast<float>(player_->getHeight());

    for (auto& g : maps_[currentMapIndex]->gateways())
    {
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
        float overlapTop  = std::max(0.0f, std::min(pos.y + ph, g.posY + gh) - std::max(pos.y, g.posY));

        // Check if overlap is sufficient in both dimensions
        if (overlapLeft >= requiredOverlapX && overlapTop >= requiredOverlapY)
        {
            return g.targetMapIndex;
        }
    }
    return -1;
}

ImVec2 World::getDirToPlayer(LivingEntity* entity)
{
    ImVec2 lePos = entity->getPosition();
    ImVec2 leCenter{lePos.x + entity->getWidth() * 0.5f, lePos.y + entity->getHeight() * 0.5f};
    ImVec2 pPos = player_->getPosition();
    ImVec2 pCenter{pPos.x + player_->getWidth() * 0.5f, pPos.y + player_->getHeight() * 0.5f};
    ImVec2 dirToPlayer = normalize(ImVec2{pCenter.x - leCenter.x, pCenter.y - leCenter.y});
    return dirToPlayer;
}

void World::clampToScreen(Entity& mover)
{
    if (screenWidth_ <= 0.0f || screenHeight_ <= 0.0f)
        return;

    ImVec2 p = mover.getPosition();

    if (p.x < 0.0f)
        p.x = 0.0f;
    if (p.y < UI_TOP_BAR_HEIGHT)
        p.y = UI_TOP_BAR_HEIGHT; // Clamp to UI bar bottom
    if (p.x + mover.getWidth() > screenWidth_)
        p.x = screenWidth_ - mover.getWidth();
    if (p.y + mover.getHeight() > screenHeight_)
        p.y = screenHeight_ - mover.getHeight();

    mover.setPosition(p.x, p.y);
}
void World::updateEntityLogic(LivingEntity* livingEntity, float dt) {
        if (!livingEntity || !livingEntity->isAlive()) return;

        auto* animationController = livingEntity->getAnimationController();
        if (!animationController) return;

        bool facingRight = (livingEntity->getFacingDir().x >= 0.0f);
        ImVec2 vel = livingEntity->getVelocity();

        // Melee
        if (livingEntity->isPerformingMeleeAttack() || animationController->isMeleeAttackAnimation())
        {

            animationController->setToMeleeAttack(facingRight);
            if (animationController->isInAttackFrame() && livingEntity->isPerformingMeleeAttack())
            {
                this->performMeleeAttack(*livingEntity);
                livingEntity->setIsPerformingMeleeAttack(false);
            }
        }
        // Ranged
        else if (livingEntity->isPerformingRangedAttack() || animationController->isRangedAttackAnimation())
        {

                bool isPlayer = (player_ && livingEntity == player_.get());
                ImVec2 aimDir;
                if (isPlayer) {
                    ImVec2 mousePos = ImGui::GetIO().MousePos;
                    ImVec2 playerPos = livingEntity->getPosition();
                    aimDir = {mousePos.x - playerPos.x, mousePos.y  -playerPos.y};
                }
                else {
                    aimDir = getDirToPlayer(livingEntity);
                }
                livingEntity->setFacingDir(aimDir);
                facingRight = (livingEntity->getFacingDir().x >= 0.0f);
                animationController->setToRangedAttack(facingRight);
            if (animationController->isInRangedAttackFrame() && livingEntity->isPerformingRangedAttack())
            {
                this->performRangedAttack(*livingEntity, aimDir);
                livingEntity->setIsPerformingRangedAttack(false);
            }
        }
        // Damaged
        else if (livingEntity->isDamaged())
        {
            livingEntity->setDamaged(false);
            animationController->setToHurt(facingRight);
        }
        // Walk/Idle
        else if (!animationController->isHurtAnimation() &&
                 !animationController->isMeleeAttackAnimation() &&
                 !animationController->isRangedAttackAnimation())
        {
            animationController->setToWalkOrIdle(vel.x, vel.y, facingRight);
        }

        animationController->update(dt);
}

void World::update(float dt)
{
    if (player_)
    {
        player_->update(dt);

        if (auto* livingPlayer = dynamic_cast<LivingEntity*>(player_.get()))
        {
            updateEntityLogic(livingPlayer, dt);
        }
    }
    //npc update
    for (auto& up : entities_)
    {
        if (!up) continue;

        if (player_ && up.get() == player_.get()) continue;

        up->update(dt);

        if (auto* livingEntity = dynamic_cast<LivingEntity*>(up.get()))
        {
            if (!livingEntity->isAlive())
            {
                livingEntity->setSolid(false);
                if (auto* ac = livingEntity->getAnimationController())
                {
                    bool facingRight = (livingEntity->getFacingDir().x >= 0.0f);
                    ac->setToDeath(facingRight);
                    ac->update(dt);
                }
                continue;
            }
            updateEntityLogic(livingEntity, dt);
        }
    }

    // pociski
    for (auto& up : entities_)
    {
        auto* proj = dynamic_cast<Projectile*>(up.get());
        if (!proj) continue;

        if (proj->isDead())
        {
            toRemove.push_back(proj);
            continue;
        }

        // Kolizje pocisków
        for (auto& other : entities_)
        {
            if (!other || other.get() == proj) continue;
            if (!other->isSolid()) continue;
            if (other.get() == proj->getOwner()) continue;

            if (intersectsAABB(*proj, *other))
            {
                if (auto* living = dynamic_cast<LivingEntity*>(other.get()))
                {
                    // Friendly Fire Check
                    if (living != proj->getOwner())
                        living->takeDamage(proj->getDamage());
                }
                proj->kill();
                toRemove.push_back(proj);
                break;
            }
        }

        if (proj->isDead()) continue;

        if (player_)
        {
            auto* livingPlayer = dynamic_cast<LivingEntity*>(getPlayer());
            if (livingPlayer && livingPlayer != proj->getOwner() && intersectsAABB(*proj, *livingPlayer))
            {
                livingPlayer->takeDamage(proj->getDamage());
                proj->kill();
                toRemove.push_back(proj);
            }
        }
    }

    // ruch gracza
    if (player_)
    {
        auto* livingPlayer = dynamic_cast<LivingEntity*>(player_.get());
        if (livingPlayer)
        {
            ImVec2 vel = livingPlayer->getVelocity();
            float  dx  = vel.x * dt;
            float  dy  = vel.y * dt;
            moveWithCollisions(*livingPlayer, dx, dy, entities_);
            clampToScreen(*player_);
        }
    }

    // ruch npc
    for (auto& up : entities_)
    {
        if (!up) continue;
        if (player_ && up.get() == player_.get()) continue;

        auto* living = dynamic_cast<LivingEntity*>(up.get());
        if (!living || !living->isAlive()) continue;

        ImVec2 oldPos = living->getPosition();
        ImVec2 vel    = living->getVelocity();
        float  dx     = vel.x * dt;
        float  dy     = vel.y * dt;

        moveWithCollisions(*living, dx, dy, entities_);

        // Kolizja z graczem
        if (player_ && intersectsAABB(*living, *player_))
        {
            living->setPosition(oldPos.x, oldPos.y);
        }
    }

    // czyszczenie
    for (auto* e : toRemove)
    {
        remove(e);
    }
    toRemove.clear();

    // Drzwi
    int aliveNpcCount = 0;
    for (auto& up : entities_)
    {
        if (!up) continue;
        if (player_ && up.get() == player_.get()) continue;

        auto* npc = dynamic_cast<Npc*>(up.get());
        if (npc && npc->isAlive()) aliveNpcCount++;
    }

    if (aliveNpcCount == 0 && !doorsUnlocked_)
    {
        for (Entity* doorEntity : doorEntities_)
        {
            if (doorEntity) doorEntity->setSolid(false);
        }
        doorsUnlocked_ = true;
    }

    this->gatewayIndex = playerInGateway();
    if (gatewayIndex >= 0)
    {
        this->newScene();
    }
}

void World::newScene()
{
    // Validate gatewayIndex before using it
    if (gatewayIndex < 0 || static_cast<size_t>(gatewayIndex) >= maps_.size())
    {
        std::cout << "Invalid gateway index: " << gatewayIndex << "\n";
        gatewayIndex = -1;
        return;
    }

    // Save the gateway side from the current map BEFORE switching
    GatewaySide entrySide = GatewaySide::Top;
    float       gatewayX  = 0.0f;
    float       gatewayY  = 0.0f;

    for (const auto& g : maps_[currentMapIndex]->gateways())
    {
        if (g.targetMapIndex == gatewayIndex)
        {
            int gwIndex = static_cast<int>(&g - &maps_[currentMapIndex]->gateways()[0]);
            entrySide   = maps_[currentMapIndex]->gatewaySide(gwIndex);
            gatewayX    = g.posX;
            gatewayY    = g.posY;
            break;
        }
    }

    entities_.clear();

    this->setCurrentMapIndex(gatewayIndex);
    doorsUnlocked_ = false;

    this->buildFromTmxMap();
    if (!maps_[currentMapIndex]->isVisited())
    {
        spawnNpcs();
    }

    // Move player to new position (player attributes are preserved!)
    if (player_)
    {
        spawnPlayerInNewScene(entrySide, gatewayX, gatewayY);
    }

    // std::cout << "Switched to map: " << gatewayIndex << "\n";
    gatewayIndex = -1;
}

void World::spawnNpcs()
{
    Map& map = *maps_[currentMapIndex];
    const auto& layers = map.getLayers();

    for (const auto& layer : layers)
    {
        if (layer->getType() != tmx::Layer::Type::Object){
            continue;
        }
        const auto* objGroup = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
        if (!objGroup)
            continue;

        for (const auto& obj : objGroup->getObjects())
        {
            float x = obj.getPosition().x;
            float y = obj.getPosition().y + UI_TOP_BAR_HEIGHT;
            // czytamy property "target" z obiektu
            int maxHp = -1;
            float size  = 1.0f;
            for (const auto& p : obj.getProperties())
            {
                if (p.getName() == "maxHp")
                {
                    maxHp = p.getIntValue();
                }
                if (p.getName() == "size")
                {
                    size = p.getFloatValue();
                }
            }
            if (obj.getName() == "Orc") {
                auto& npc = spawnNpc(NpcType::Orc, {x, y});
                if (maxHp > 0)
                {
                    npc.setHp(maxHp); 
                    npc.setSpriteScale(size);
                }
            }
            else if (obj.getName() == "Skeleton_Archer")
            {
                auto& npc = spawnNpc(NpcType::Skeleton_Archer, {x, y});
                if (maxHp > 0)
                {
                    npc.setHp(maxHp);
                    npc.setSpriteScale(size);
                }
            }
            else if (obj.getName() == "Knight")
            {
                auto& npc = spawnNpc(NpcType::Knight, {x, y});
                if (maxHp > 0)
                {
                    npc.setHp(maxHp);
                    npc.setSpriteScale(size);
                }
            }
            else if (obj.getName() == "EliteOrc")
            {
                auto& npc = spawnNpc(NpcType::Elite_Orc, {x, y});
                if (maxHp > 0)
                {
                    npc.setHp(maxHp);
                    npc.setSpriteScale(size);
                }
            }
        }
        
    }
    maps_[currentMapIndex]->setVisited(true);
}

void World::spawnPlayerInNewScene(GatewaySide entrySide, float sourceGatewayX, float sourceGatewayY)
{
    constexpr float tile = 64.0f;

    int mapRows = maps_[currentMapIndex]->getRows();
    int mapCols = maps_[currentMapIndex]->getColumns();
    float mapWidth = mapCols * tile;
    float mapHeight = mapRows * tile;

    switch (entrySide)
    {
    case GatewaySide::Top:
        // Entering from top, exit at bottom - spawn as close to bottom edge as possible
        sourceGatewayX = player_->getPosition().x;
        player_->setPosition(sourceGatewayX, mapHeight + UI_TOP_BAR_HEIGHT -tile*4);
        break;
    case GatewaySide::Bottom:
        // Entering from bottom, exit at top - spawn as close to top edge as possible (after UI bar)
        sourceGatewayX = player_->getPosition().x;
        player_->setPosition(sourceGatewayX, UI_TOP_BAR_HEIGHT + tile*3);
        break;
    case GatewaySide::Left:
        // Entering from left, exit at right - spawn as close to right edge as possible
        sourceGatewayY = player_->getPosition().y;
        player_->setPosition(mapWidth - tile*4, sourceGatewayY);
        break;
    case GatewaySide::Right:
        // Entering from right, exit at left - spawn as close to left edge as possible
        sourceGatewayY = player_->getPosition().y;
        player_->setPosition(tile * 3, sourceGatewayY);
        break;
    }
}

void World::clear()
{
    entities_.clear();
    player_ = nullptr;
}

void World::forEachEntity(const std::function<void(Entity&)>& fn)
{
    for (auto& up : entities_)
    {
        if (up)
            fn(*up);
    }
}

bool World::remove(Entity* ptr)
{
    if (!ptr)
        return false;

    if (player_.get() == ptr)
    {
        player_.reset();
        return true;
    }

    auto       it      = std::remove_if(entities_.begin(), entities_.end(),
                                        [&](const std::unique_ptr<Entity>& up) { return up.get() == ptr; });
    const bool removed = (it != entities_.end());
    entities_.erase(it, entities_.end());
    return removed;
}

void World::addMapfromTmx(const std::string& path) {
    auto m = std::make_unique<Map>();
    if (!m->loadFromTmxFile(path,assets_))
    {
        throw std::runtime_error("Could not load TMX map file: " + path);
    }
    maps_.push_back(std::move(m));
}

int World::getConsumableIconId(ConsumableType type)
{
    if (!assets_)
        return -1;

    auto it = consumableIconIds_.find(type);
    if (it != consumableIconIds_.end())
        return it->second;

    std::string path;
    switch (type)
    {
        case ConsumableType::HealthPotion:   path = "assets/items/consumable/HealthPotion.png"; break;
        case ConsumableType::SpeedPotion:    path = "assets/items/consumable/SpeedPotion.png"; break;
        case ConsumableType::StrengthPotion: path = "assets/items/consumable/StrengthPotion.png"; break;
    }

    int iconId = assets_->getOrLoadIcon(path);
    consumableIconIds_[type] = iconId;
    return iconId;
}

void World::givePlayerConsumable(ConsumableType type)
{
    if (!player_ || !assets_)
        return;

    int iconId = getConsumableIconId(type);

    std::unique_ptr<Item> item;
    switch (type)
    {
        case ConsumableType::HealthPotion:
            item = std::make_unique<HealthPotion>();
            break;
        case ConsumableType::SpeedPotion:
            item = std::make_unique<SpeedPotion>();
            break;
        case ConsumableType::StrengthPotion:
            item = std::make_unique<StrengthPotion>();
            break;
    }

    if (item)
    {
        item->setIconId(iconId);
        player_->getInventory().addItem(std::move(item));
    }
}

void World::reset() {
    this->clear();
    for ( auto&& map: maps_)
    {
        map->setVisited(false);
    }
}
