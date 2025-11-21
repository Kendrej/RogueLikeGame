
#pragma once

#include <imgui.h>
#include <cmath>


inline float distance(const ImVec2& a, const ImVec2& b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return std::sqrt(dx*dx + dy*dy);
}
inline ImVec2 normalize(const ImVec2& v) {
    float len = std::sqrt(v.x*v.x + v.y*v.y);
    if (len < 0.0001f) return ImVec2(0,0);
    return ImVec2(v.x/len, v.y/len);
}