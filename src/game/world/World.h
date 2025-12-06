#pragma once
#include "game/entities/Entity.h"
#include "game/world/Map.h"
#include "game/entities/Player.h"
#include "game/entities/StaticEntity.h"
#include "game/npc/Npc.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

enum class NpcType;
class Projectile;
class Assets;
class StaticEntity;
class Entity;
class Npc;
class World
{
public:
    static constexpr float UI_TOP_BAR_HEIGHT = 56.0f; // Black bar at top for UI (1080 - 16*64 = 56px)

    explicit World(Assets* assets) noexcept : assets_(assets) {}

    StaticEntity& spawnTile(const std::string& texturePath, uint32_t width, uint32_t height, float pos_x, float pos_y,
                            bool solid);
 
    Player& spawnPlayer(ImVec2 pos);

    Npc& spawnNpc(NpcType type, ImVec2 pos);

    Projectile& spawnProjectile(uint32_t width, uint32_t height, float pos_x, float pos_y, ImVec2 velocity,
                                float lifetime, int damage, LivingEntity* owner, const std::string& texturePath);

    void performMeleeAttack(LivingEntity& attacker);
    void performRangedAttack(LivingEntity& attacker);

    void buildFromMap(const std::string& wallTexturePath, const std::string& floorTexturePath,
                      const std::string& doorTexturePath, uint32_t tileW, uint32_t tileH);

    void buildFromTmxMap();

    void update(float dt);
    void clear();
    void newScene();

    void setScreenBounds(float width, float height)
    {
        screenWidth_  = width;
        screenHeight_ = height;
    }

    Map* getMap(size_t index) const noexcept
    {
        if (index < maps_.size())
            return maps_[index].get();
        return nullptr;
    }
    Player* getPlayer() const noexcept
    {
        return player_.get();
    }
    const std::vector<std::unique_ptr<Entity>>& entities() const noexcept
    {
        return entities_;
    }
    void forEachEntity(const std::function<void(Entity&)>& fn);
    bool remove(Entity* ptr);
    void setCurrentMapIndex(int index)
    {
        currentMapIndex = index;
    }
    int getCurrentMapIndex() const
    {
        return currentMapIndex;
    }
    void addMap(const std::string& path);
    void addMapfromTmx(const std::string& path);
    int playerInGateway();
    Assets* getAssets() const noexcept
    {
        return assets_;
    }
    ImVec2 getDirToPlayer(LivingEntity* entity);

private:
    template <class T, class... Args> T& addEntity(Args&&... args)
    {
        auto up  = std::make_unique<T>(std::forward<Args>(args)...);
        T&   ref = *up;
        entities_.emplace_back(std::move(up));
        return ref;
    }

    // Collision helpers (renamed for clarity)
    static bool intersectsAABB(const Entity& a, const Entity& b);
    static bool intersectsAABBAt(const Entity& a, const Entity& b, float aPosX, float aPosY);
    static bool intersectsRectWithEntity(const Entity& e, float rx, float ry, float rw, float rh);
    static void moveWithCollisions(Entity& mover, float dx, float dy,
                                   const std::vector<std::unique_ptr<Entity>>& entities);
    static void pushOutOfSolids(Entity& mover, const std::vector<std::unique_ptr<Entity>>& entities);
    void        clampToScreen(Entity& mover);
    GatewaySide getSide(int gatewayIndex);
    void        spawnPlayerInNewScene(GatewaySide entrySide, float sourceGatewayX, float sourceGatewayY);
    void        spawnNpcs();
    int         gatewayIndex    = -1;
    int         currentMapIndex = 0;
    std::vector<std::unique_ptr<Map>>    maps_;
    std::vector<int>                     gatewayIndexes_;
    Assets*                              assets_{nullptr};
    std::vector<std::unique_ptr<Entity>> entities_;
    std::unique_ptr<Player>              player_{nullptr};
    float                                screenWidth_   = 0.0f;
    float                                screenHeight_  = 0.0f;
    bool                                 doorsUnlocked_ = false;
};
