#pragma once
#include "game/entities/Entity.h"
#include "game/world/Map.h"
#include "game/entities/Player.h"
#include "game/npc/Npc.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

enum class ItemId;
enum class NpcType;
class Projectile;
class Assets;
class Entity;
class Npc;
class LivingEntity;

class World
{
public:
    static constexpr float UI_TOP_BAR_HEIGHT = 56.0f; // Black bar at top for UI (1080 - 16*64 = 56px)

    explicit World(Assets* assets) noexcept : assets_(assets) {}

    Entity& spawnTile(const std::string& texturePath, uint32_t width, uint32_t height, float pos_x, float pos_y,
                            bool solid);
 
    Player& spawnPlayer(ImVec2 pos);

    Npc& spawnNpc(NpcType type, ImVec2 pos);

    Item& spawnItem(ItemId id, float x, float y);

    Projectile& spawnProjectile(uint32_t width, uint32_t height, float pos_x, float pos_y, ImVec2 velocity,
                                float lifetime, int damage, LivingEntity* owner, const std::string& texturePath);

    void performMeleeAttack(LivingEntity& attacker);
    void performRangedAttack(LivingEntity& attacker, ImVec2 direction);

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
    void addMapfromTmx(const std::string& path);
    int playerInGateway();
    Assets* getAssets() const noexcept
    {
        return assets_;
    }
    ImVec2 getDirToPlayer(LivingEntity* entity);
    void        reset();


    enum class ConsumableType { HealthPotion, SpeedPotion, StrengthPotion };
    void        givePlayerConsumable(ConsumableType type);
    int         getConsumableIconId(ConsumableType type);


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
    GatewaySide getSide(int gwIndex);
    void        spawnPlayerInNewScene(GatewaySide entrySide, float sourceGatewayX, float sourceGatewayY);
    void        updateEntityLogic(LivingEntity* livingEntity, float dt);
    void        spawnNpcs();

    int currentMapIndex = 0;
    int gatewayIndex    = -1;
    std::unordered_map<ConsumableType, int> consumableIconIds_;
    std::vector<std::unique_ptr<Map>> maps_;
    std::vector<Entity*> doorEntities_;
    Assets* assets_{nullptr};
    std::vector<std::unique_ptr<Entity>> entities_;
    std::unique_ptr<Player> player_{nullptr};
    struct AnimatedTile
    {
        Entity* entity = nullptr;
        const std::vector<std::uint32_t>* frames = nullptr; // pointer into Map's animatedFrames_
        int frameIndex = 0;
        float timer = 0.0f;
        float frameDuration = 0.1f; // default 100ms
        std::string use; // optional property value (e.g., "door")
        bool oneTimeAnimationDone = false;
    };
    std::vector<AnimatedTile> animatedTiles_;
    float screenWidth_ = 0.0f;
    float screenHeight_ = 0.0f;
    bool doorsUnlocked_ = false;
    std::vector<Entity*> toRemove;
};
