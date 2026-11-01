// Testy AiEnvironment dla sciezek, do ktorych losowa gra praktycznie nie dochodzi:
// klucz z bossa, otwieranie zamknietych drzwi i wygrana.
#include <iostream>
#include <optional>

#include <imgui.h>

#include "ai/AiEnvironment.h"
#include "app/GameSetup.h"
#include "engine/gfx/Assets.h"
#include "game/entities/Player.h"
#include "game/factory/ItemFactory.h"
#include "game/item/Inventory.h"
#include "game/npc/Npc.h"
#include "game/world/World.h"

namespace
{
constexpr float DT         = 1.0f / 60.0f;
constexpr int   MAX_STEPS  = 600;
int             failures   = 0;

void check(const char* name, bool ok)
{
    std::cout << (ok ? "OK   " : "FAIL ") << name << std::endl;
    if (!ok)
        failures++;
}

void loadMap(World& world, int level, int index)
{
    world.reset();
    world.setCurrentMapLevel(level);
    world.setCurrentMapIndex(index);
    world.buildFromTmxMap();
    world.spawnNpcs();
    world.spawnPlayer({250.0f, 250.0f + World::UI_TOP_BAR_HEIGHT});
}

// Stawia gracza przy NPC spelniajacym warunek, oslabia go i bije do skutku. Zwraca sume zdarzen.
template <typename Pred> nlohmann::json killNpc(World& world, AiEnvironment& env, Pred pred)
{
    Npc* target = nullptr;
    for (const auto& up : world.entities())
    {
        auto* npc = dynamic_cast<Npc*>(up.get());
        if (npc && npc->isAlive() && pred(*npc))
            target = npc;
    }
    nlohmann::json totals = {{"enemiesKilled", 0}, {"keysPicked", 0}, {"won", false}, {"found", target != nullptr}};
    if (!target)
        return totals;

    target->setHp(1);
    for (int i = 0; i < MAX_STEPS && target->isAlive(); ++i)
    {
        Player* player = world.getPlayer();
        player->setHp(player->getMaxHp());
        player->setPosition(target->getPosition().x - static_cast<float>(player->getWidth()), target->getPosition().y);
        player->setFacingDir(ImVec2(1.0f, 0.0f));
        env.step(4, 4, DT);
        auto state = env.consumeState();
        totals["enemiesKilled"] = totals["enemiesKilled"].get<int>() + state["events"]["enemiesKilled"].get<int>();
        totals["keysPicked"]    = totals["keysPicked"].get<int>() + state["events"]["keysPicked"].get<int>();
    }
    for (int i = 0; i < 5; ++i)
    {
        env.step(0, 1, DT);
        auto state = env.consumeState();
        totals["enemiesKilled"] = totals["enemiesKilled"].get<int>() + state["events"]["enemiesKilled"].get<int>();
        totals["keysPicked"]    = totals["keysPicked"].get<int>() + state["events"]["keysPicked"].get<int>();
        totals["won"]           = state["won"];
    }
    return totals;
}
} // namespace

int main()
{
    ImGui::CreateContext();
    Assets assets(Assets::Ctx{});
    World  world(&assets);
    world.setScreenBounds(1920.0f, 1080.0f);
    setupGame(world);
    std::optional<AiEnvironment> env;

    // Klucz wypada z bossa na mapie 0/4
    loadMap(world, 0, 4);
    env.emplace(world);
    auto keyRun = killNpc(world, *env, [](Npc& npc) { return npc.getGivingItem() == ItemId::Key; });
    check("boss z kluczem istnieje", keyRun["found"].get<bool>());
    check("zabicie bossa liczy sie raz", keyRun["enemiesKilled"].get<int>() == 1);
    check("podniesienie klucza liczy sie raz", keyRun["keysPicked"].get<int>() == 1);
    check("stan zglasza klucz", env->consumeState()["key"].get<bool>());

    // Klucz otwiera drzwi na mapie, ktora je ma
    bool doorTested = false;
    for (int level = 0; level < 2 && !doorTested; ++level)
    {
        for (int index = 0; index < 5 && !doorTested; ++index)
        {
            auto* map = world.getMap(level, index);
            if (!map || !map->isLockedDoors())
                continue;

            doorTested = true;
            loadMap(world, level, index);
            map = world.getMap(level, index);
            env.emplace(world);
            world.givePlayerItem(ItemId::Key);
            env->step(0, 1, DT); // zauwaza klucz
            auto picked = env->consumeState();
            check("klucz dany graczowi jest wykryty", picked["events"]["keysPicked"].get<int>() == 1);

            LockedDoorInfo info = map->getLockedDoorInfo();
            world.getPlayer()->setPosition(info.posX, info.posY);
            env->step(4, 1, DT);
            auto used = env->consumeState();
            check("uzycie klucza liczy sie raz", used["events"]["keysUsed"].get<int>() == 1);
            check("drzwi sa otwarte", !map->isLockedDoors());
            check("klucz zniknal ze stanu", !used["key"].get<bool>());

            int keysLeft = 0;
            auto& inventory = world.getPlayer()->getInventory();
            for (int i = 0; i < inventory.getItemCount(); ++i)
                keysLeft += inventory.getItem(i) && inventory.getItem(i)->getName() == "Key";
            check("klucz zniknal z ekwipunku", keysLeft == 0);
        }
    }
    check("znaleziono mape z zamknietymi drzwiami", doorTested);

    // Reset czysci drzwi i klucz
    env->reset();
    auto resetState = env->consumeState();
    check("reset zeruje klucz", !resetState["key"].get<bool>());
    check("reset zeruje zdarzenia", resetState["events"]["keysUsed"].get<int>() == 0);

    // Wygrana po bossie na mapie 1/4
    loadMap(world, 1, 4);
    env.emplace(world);
    auto winRun = killNpc(world, *env, [](Npc& npc) { return npc.isWinAfterDeath(); });
    check("boss koncowy istnieje", winRun["found"].get<bool>());
    check("smierc bossa konczy gre wygrana", winRun["won"].get<bool>());

    ImGui::DestroyContext();
    std::cout << (failures ? "NIEZALICZONE" : "WSZYSTKO OK") << std::endl;
    return failures ? 1 : 0;
}
