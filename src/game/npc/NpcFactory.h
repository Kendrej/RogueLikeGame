#pragma once
#include <imgui.h>

class Npc;
class World;

enum class NpcType {
    Skeleton_Archer, Orc
};

class NpcFactory {
public:
    static Npc* createNpc(NpcType type, World& world, ImVec2 pos);
};

