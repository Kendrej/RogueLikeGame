#pragma once
#include <imgui.h>

class Npc;
class World;

enum class NpcType {
    Skeleton_Archer, Orc, Knight, Elite_Orc
};

class NpcFactory {
public:
    static Npc* createNpc(NpcType type, World& world, ImVec2 pos);
};

