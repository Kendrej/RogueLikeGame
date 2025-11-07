#pragma once
#include <memory>
#include <vector>
#include <string>
#include <cstdint>
#include <functional>

class Entity;
class Player;
class Assets;

class World {
public:
    explicit World(Assets* assets) noexcept : assets_(assets) {}


    Entity& spawnTile(const std::string& texturePath,
                      uint32_t width, uint32_t height,
                      float pos_x, float pos_y);

    Player& spawnPlayer(const std::string& texturePath,
                        uint32_t width, uint32_t height,
                        float pos_x, float pos_y);


    void update(float dt);
    void clear();

    Player* getPlayer() const noexcept { return player_; }
    const std::vector<std::unique_ptr<Entity>>& entities() const noexcept { return entities_; }
    void forEachEntity(const std::function<void(Entity&)>& fn);
    bool remove(Entity* ptr);

private:
    template <class T, class... Args>
    T& addEntity(Args&&... args) {
        auto up = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *up;
        entities_.emplace_back(std::move(up));
        return ref;
    }

private:
    Assets* assets_{nullptr};
    std::vector<std::unique_ptr<Entity>> entities_;
    Player* player_{nullptr};
};
