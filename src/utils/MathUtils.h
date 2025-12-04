
#pragma once

#include <cmath>
#include <imgui.h>
#include "game/npc/Npc.h"
#include "game/world/World.h"

inline float distance(const ImVec2& a, const ImVec2& b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return std::sqrt(dx * dx + dy * dy);
}
inline ImVec2 normalize(const ImVec2& v)
{
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len < 0.0001f)
        return {0, 0};
    return {v.x / len, v.y / len};
}


inline bool hasObstacleInDirection(const Npc& npc, const World& world, ImVec2 direction, float checkDistance)
    {
        float dirLen = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (dirLen < 0.001f)
            return false;

        ImVec2 npcPos = npc.getPosition();
        ImVec2 checkPos = ImVec2{
            npcPos.x + direction.x * checkDistance,
            npcPos.y + direction.y * checkDistance
        };

        for (const auto& entity : world.entities())
        {
            if (!entity || entity.get() == &npc)
                continue;
            if (!entity->isSolid())
                continue;

            ImVec2 entityPos = entity->getPosition();
            float ex1 = entityPos.x;
            float ey1 = entityPos.y;
            float ex2 = entityPos.x + static_cast<float>(entity->getWidth());
            float ey2 = entityPos.y + static_cast<float>(entity->getHeight());

            float nx1 = checkPos.x;
            float ny1 = checkPos.y;
            float nx2 = checkPos.x + static_cast<float>(npc.getWidth());
            float ny2 = checkPos.y + static_cast<float>(npc.getHeight());

            if (!(nx1 >= ex2 || nx2 <= ex1 || ny1 >= ey2 || ny2 <= ey1))
            {
                return true;
            }
        }
        return false;
    }

inline ImVec2 findAvoidanceDirection(const Npc& npc, World& world, ImVec2 desiredDir, float checkDistance)
    {
        if (!hasObstacleInDirection(npc, world, desiredDir, checkDistance))
        {
            return desiredDir;
        }

        float angles[] = {45.0f, -45.0f, 90.0f, -90.0f, 135.0f, -135.0f};

        for (float angle : angles)
        {
            float radians = angle * 3.14159f / 180.0f;
            float cosA = cosf(radians);
            float sinA = sinf(radians);

            ImVec2 rotatedDir = ImVec2{
                desiredDir.x * cosA - desiredDir.y * sinA,
                desiredDir.x * sinA + desiredDir.y * cosA
            };

            if (!hasObstacleInDirection(npc, world, rotatedDir, checkDistance))
            {
                return normalize(rotatedDir);
            }
        }

        return normalize(ImVec2{-desiredDir.x, -desiredDir.y});
    }