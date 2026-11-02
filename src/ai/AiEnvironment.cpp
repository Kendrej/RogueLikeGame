#include "ai/AiEnvironment.h"

#include <cmath>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "app/GameSetup.h"
#include "game/entities/Player.h"
#include "game/item/Inventory.h"
#include "game/npc/Npc.h"
#include "game/world/World.h"

namespace
{
constexpr float TILE_SIZE     = 64.0f;
constexpr int   MAX_ENEMIES   = 5;

struct EnemyInfo
{
    int x;
    int y;
    int type;
    int hp;
};

std::vector<EnemyInfo> collectEnemies(World& world)
{
    std::vector<EnemyInfo> enemies;
    for (const auto& up : world.entities())
    {
        if (!up || up.get() == world.getPlayer())
            continue;
        auto* npc = dynamic_cast<Npc*>(up.get());
        if (npc && npc->isAlive())
        {
            ImVec2 pos = npc->getPosition();
            enemies.push_back(EnemyInfo{static_cast<int>(pos.x / TILE_SIZE),
                                        static_cast<int>((pos.y - World::UI_TOP_BAR_HEIGHT) / TILE_SIZE),
                                        npc->getAttackClass() + 1, npc->getHp()});
        }
    }
    return enemies;
}

int findKeySlot(Inventory& inventory)
{
    for (int i = 0; i < inventory.getItemCount(); i++)
    {
        Item* item = inventory.getItem(i);
        if (item && item->getName() == "Key")
            return i;
    }
    return -1;
}
} // namespace

AiEnvironment::AiEnvironment(World& world, std::function<void()> beforeReset)
    : world_(world), beforeReset_(std::move(beforeReset))
{
    resetTrackers();
}

void AiEnvironment::reset()
{
    if (beforeReset_)
        beforeReset_();
    restartGame(world_);
    resetTrackers();
}

void AiEnvironment::resetTrackers()
{
    events_         = Events{};
    lastPlayerHp_   = -1;
    lastEnemyCount_ = -1;
    lastEnemiesHp_  = -1;
    hasKey_         = false;
    lastMapIndex_   = world_.getCurrentMapIndex();
    lastMapLevel_   = world_.getCurrentMapLevel();
    std::memset(visitedTiles_, 0, sizeof(visitedTiles_));
}

void AiEnvironment::step(int action, int frameSkip, float dt)
{
    for (int tick = 0; tick < frameSkip; ++tick)
    {
        Player* player = world_.getPlayer();
        if (!player || !player->isAlive() || world_.isGameWon())
            break;

        applyAction(action);
        useKeyOnDoor();
        world_.update(dt);
        trackEvents();
    }
}

void AiEnvironment::applyAction(int action)
{
    Player* player = world_.getPlayer();
    float   dx = 0.0f, dy = 0.0f;
    switch (action)
    {
        case 0: dy = -1.0f; break;
        case 1: dy =  1.0f; break;
        case 2: dx = -1.0f; break;
        case 3: dx =  1.0f; break;
        default: break;
    }
    player->applyInput(ImVec2(dx, dy));

    if (action == 4 && player->isMeleeMode() && player->canMelee())
    {
        if (!player->getAnimationController())
            world_.performMeleeAttack(*player);
        else
            player->setIsPerformingMeleeAttack(true);
        player->startMeleeCooldown();
    }
    if (action == 5 && player->canShoot())
        shootNearestEnemy();
}

void AiEnvironment::shootNearestEnemy()
{
    // Gra dla czlowieka celuje myszka, ktorej w symulatorze nie ma, wiec agent dostaje auto-cel
    Player* player = world_.getPlayer();
    ImVec2  p      = player->getPosition();
    ImVec2  from{p.x + 0.5f * static_cast<float>(player->getWidth()), p.y + 0.5f * static_cast<float>(player->getHeight())};
    ImVec2  aim{0.0f, 0.0f};
    float   best = -1.0f;
    for (const auto& up : world_.entities())
    {
        auto* npc = dynamic_cast<Npc*>(up.get());
        if (!npc || !npc->isAlive())
            continue;
        ImVec2 q = npc->getPosition();
        float  dx = q.x + 0.5f * static_cast<float>(npc->getWidth()) - from.x;
        float  dy = q.y + 0.5f * static_cast<float>(npc->getHeight()) - from.y;
        float  d  = dx * dx + dy * dy;
        if (best < 0.0f || d < best)
        {
            best = d;
            aim  = ImVec2(dx, dy);
        }
    }
    if (best <= 0.0f)
        return;
    world_.performRangedAttack(*player, aim);
    player->startRangedCooldown();
}

void AiEnvironment::useKeyOnDoor()
{
    if (!hasKey_)
        return;

    Player* player = world_.getPlayer();
    auto*   map    = world_.getMap(world_.getCurrentMapLevel(), world_.getCurrentMapIndex());
    if (!map || !map->isLockedDoors())
        return;

    ImVec2         p    = player->getPosition();
    float          pw   = static_cast<float>(player->getWidth());
    float          ph   = static_cast<float>(player->getHeight());
    LockedDoorInfo info = map->getLockedDoorInfo();
    if (p.x + pw > info.posX && p.x < info.posX + info.width && p.y + ph > info.posY && p.y < info.posY + info.height)
    {
        auto& inventory = player->getInventory();
        int   slot      = findKeySlot(inventory);
        if (slot >= 0)
        {
            inventory.removeItem(slot);
            map->setLockedDoors(false);
            events_.keysUsed++;
        }
        hasKey_ = false;
    }
}

void AiEnvironment::trackEvents()
{
    Player* player = world_.getPlayer();

    if (!hasKey_ && findKeySlot(player->getInventory()) >= 0)
    {
        hasKey_ = true;
        events_.keysPicked++;
    }

    int  currentMap = world_.getCurrentMapIndex();
    int  currentLevel = world_.getCurrentMapLevel();
    bool mapChanged = currentMap != lastMapIndex_ || currentLevel != lastMapLevel_;
    if (mapChanged)
    {
        events_.mapsChanged++;
        lastMapIndex_ = currentMap;
        lastMapLevel_ = currentLevel;
        std::memset(visitedTiles_, 0, sizeof(visitedTiles_));
    }

    // Zmiana mapy podmienia wrogow, wiec roznica nie oznacza zabojstw ani obrazen
    auto enemies = collectEnemies(world_);
    int  count   = static_cast<int>(enemies.size());
    int  enemyHp = 0;
    for (const auto& e : enemies)
        enemyHp += e.hp;
    if (!mapChanged && lastEnemyCount_ >= 0)
    {
        if (count < lastEnemyCount_)
            events_.enemiesKilled += lastEnemyCount_ - count;
        if (enemyHp < lastEnemiesHp_)
            events_.damageDealt += lastEnemiesHp_ - enemyHp;
        if (count == 0 && lastEnemyCount_ > 0)
            events_.roomsCleared++;
    }
    lastEnemyCount_ = count;
    lastEnemiesHp_  = enemyHp;

    int hp = player->getHp();
    if (lastPlayerHp_ != -1)
        events_.hpDelta += hp - lastPlayerHp_;
    lastPlayerHp_ = hp;

    ImVec2 pos   = player->getPosition();
    int    tileX = static_cast<int>(pos.x / TILE_SIZE);
    int    tileY = static_cast<int>((pos.y - World::UI_TOP_BAR_HEIGHT) / TILE_SIZE);
    if (tileX >= 0 && tileX < 30 && tileY >= 0 && tileY < 16 && !visitedTiles_[tileX][tileY])
    {
        visitedTiles_[tileX][tileY] = true;
        events_.newTiles++;
    }
}

nlohmann::json AiEnvironment::consumeState()
{
    Player* player = world_.getPlayer();
    ImVec2  pos    = player->getPosition();

    nlohmann::json enemiesJson = nlohmann::json::array();
    auto           enemies     = collectEnemies(world_);
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (i < static_cast<int>(enemies.size()))
            enemiesJson.push_back({{"x", enemies[i].x}, {"y", enemies[i].y}, {"type", enemies[i].type}, {"hp", enemies[i].hp}});
        else
            enemiesJson.push_back({{"x", -1}, {"y", -1}, {"type", -1}, {"hp", -1}});
    }

    nlohmann::json itemsJson = nlohmann::json::array();
    for (const auto& up : world_.entities())
    {
        if (up && up->isItem())
        {
            ImVec2 itemPos = up->getPosition();
            itemsJson.push_back({{"x", static_cast<int>(itemPos.x / TILE_SIZE)},
                                 {"y", static_cast<int>((itemPos.y - World::UI_TOP_BAR_HEIGHT) / TILE_SIZE)}});
        }
    }

    nlohmann::json gatewaysJson = nlohmann::json::array();
    auto*          map          = world_.getMap(world_.getCurrentMapLevel(), world_.getCurrentMapIndex());
    if (map)
    {
        for (const auto& g : map->gateways())
        {
            gatewaysJson.push_back({{"x", static_cast<int>(g.posX / TILE_SIZE)},
                                    {"y", static_cast<int>((g.posY - World::UI_TOP_BAR_HEIGHT) / TILE_SIZE)},
                                    {"level", g.level},
                                    {"map", g.targetMapIndex}});
        }
    }

    nlohmann::json state;
    state["protocol"]   = PROTOCOL_VERSION;
    state["enemyCount"] = static_cast<int>(enemies.size());
    state["items"]      = itemsJson;
    state["gateways"]   = gatewaysJson;
    state["lockedDoor"] = map && map->isLockedDoors();
    state["facingX"]    = player->getFacingDir().x;
    state["facingY"]    = player->getFacingDir().y;
    state["meleeReady"] = player->canMelee();
    state["rangedReady"] = player->canShoot();
    state["fracX"]      = pos.x / TILE_SIZE - std::floor(pos.x / TILE_SIZE);
    state["fracY"]      = (pos.y - World::UI_TOP_BAR_HEIGHT) / TILE_SIZE - std::floor((pos.y - World::UI_TOP_BAR_HEIGHT) / TILE_SIZE);
    state["hp"]       = player->getHp();
    state["alive"]    = player->isAlive();
    state["won"]      = world_.isGameWon();
    state["playerX"]  = static_cast<int>(pos.x / TILE_SIZE);
    state["playerY"]  = static_cast<int>((pos.y - World::UI_TOP_BAR_HEIGHT) / TILE_SIZE);
    state["key"]      = hasKey_;
    state["map"]      = world_.getCurrentMapIndex();
    state["level"]    = world_.getCurrentMapLevel();
    state["grid"]     = world_.getTileGrid();
    state["enemies"]  = enemiesJson;
    state["events"]   = {{"keysPicked", events_.keysPicked},
                         {"keysUsed", events_.keysUsed},
                         {"enemiesKilled", events_.enemiesKilled},
                         {"damageDealt", events_.damageDealt},
                         {"hpDelta", events_.hpDelta},
                         {"newTiles", events_.newTiles},
                         {"mapsChanged", events_.mapsChanged},
                         {"roomsCleared", events_.roomsCleared}};
    events_ = Events{};
    return state;
}
