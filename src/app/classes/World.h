#pragma once
#include <memory>
#include <vector>
#include <string>
#include <cstdint>
#include <functional>
#include "Entity.h"
#include "Map.h" // added to ensure Map is a complete type for unique_ptr

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

    void buildFromMap(const Map& map,
                      const std::string& wallTexturePath,
                      const std::string& floorTexturePath,
                      uint32_t tileW, uint32_t tileH)
    ;
    void update(float dt);
    void clear();

    Map* getMap(size_t index) const noexcept {
        if (index < maps_.size()) return maps_[index].get();
        return nullptr;
	}
    Player* getPlayer() const noexcept { return player_; }
    const std::vector<std::unique_ptr<Entity>>& entities() const noexcept { return entities_; }
    void forEachEntity(const std::function<void(Entity&)>& fn);
    bool remove(Entity* ptr);
	void setCurrentMapIndex(int index) { currentMapIndex = index; }
	int getCurrentMapIndex() const { return currentMapIndex; }
private:
    template <class T, class... Args>
    T& addEntity(Args&&... args) {
        auto up = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *up;
        entities_.emplace_back(std::move(up));
        return ref;
    }
	int currentMapIndex = 0;
    std::vector<std::unique_ptr<Map>> maps_;
    Assets* assets_{nullptr};
    std::vector<std::unique_ptr<Entity>> entities_;
    Player* player_{nullptr};
};
