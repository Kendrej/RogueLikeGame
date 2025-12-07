#pragma once
#include <cstdint>
#include <imgui.h>

class Assets;

class Entity
{
public:
    Entity(int entityId, uint32_t width, uint32_t height, float pos_x, float pos_y);
    virtual ~Entity() = default;

    ImVec2   getPosition() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    int      getEntityId() const;
    bool     isVisible() const;

    void setVisible(bool visible);
    void setPosition(float x, float y);

    virtual void update(float /*dt*/) {}
    void         moveBy(float dx, float dy);

    bool isSolid() const
    {
        return solid_;
    }
    void setSolid(bool solid)
{
        solid_ = solid;
    }

    // Lewy gorny rog kafelka w spritesheet (w pikselach)
    void setTexOffset(uint32_t x, uint32_t y)
    {
        texX_ = x;
        texY_ = y;
    }
    uint32_t getTexX() const { return texX_; }
    uint32_t getTexY() const { return texY_; }

protected:
    int      entityId;
    uint32_t width   = 0;
    uint32_t height  = 0;
    bool     visible = true;
    bool     solid_  = false;

    // Lewy gorny rog w spritesheet (0,0 = pelna tekstura)
    uint32_t texX_ = 0;
    uint32_t texY_ = 0;

private:
    ImVec2 pos{0.0f, 0.0f};
};
