#pragma once

#include <functional>
#include <nlohmann/json.hpp>

class World;

// Logika sterowania gra przez agenta, niezalezna od grafiki.
// Jeden krok agenta to frameSkip tickow symulacji o stalym dt.
class AiEnvironment
{
public:
    static constexpr int PROTOCOL_VERSION = 5;
    static constexpr int ACTION_COUNT     = 6; // 0-3 ruch, 4 atak wrecz, 5 strzal w najblizszego wroga

    // beforeReset pozwala aplikacji graficznej poczekac na GPU przed przebudowa swiata
    explicit AiEnvironment(World& world, std::function<void()> beforeReset = {});

    void reset();
    void step(int action, int frameSkip, float dt);

    // Surowy stan i zdarzenia od poprzedniego wywolania. Nagrode liczy klient.
    nlohmann::json consumeState();

private:
    struct Events
    {
        int  keysPicked    = 0;
        int  keysUsed      = 0;
        int  enemiesKilled = 0;
        int  damageDealt   = 0;
        int  hpDelta       = 0;
        int  newTiles      = 0;
        int  mapsChanged   = 0;
        int  roomsCleared  = 0; // ostatni wrog na mapie zginal, drzwi sie otwieraja
    };

    void applyAction(int action);
    void shootNearestEnemy();
    void useKeyOnDoor();
    void trackEvents();
    void resetTrackers();

    World&                world_;
    std::function<void()> beforeReset_;
    Events                events_;

    int  lastPlayerHp_   = -1;
    int  lastMapIndex_   = 0;
    int  lastMapLevel_   = 0;
    int  lastEnemyCount_ = -1;
    int  lastEnemiesHp_  = -1;
    bool hasKey_         = false;
    bool visitedTiles_[30][16] = {};
};
