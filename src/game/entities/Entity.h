#pragma once
#include <cstdint>
#include <imgui.h>
#include <memory>



class Assets;
class Item;

class Entity
{
public:
    Entity(int entityId, uint32_t width, uint32_t height, float pos_x, float pos_y, bool solid = false);
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

    void setTexOffset(uint32_t x, uint32_t y)
    {
        texX_ = x;
        texY_ = y;
    }
    uint32_t getTexX() const { return texX_; }
    uint32_t getTexY() const { return texY_; }

    void setFlipFlags(bool flipH, bool flipV, bool flipD)
    {
        flipH_ = flipH;
        flipV_ = flipV;
        flipD_ = flipD;
    }
    bool getFlipH() const { return flipH_; }
    bool getFlipV() const { return flipV_; }
    bool getFlipD() const { return flipD_; }

    void setDroppingItem(std::unique_ptr<Item> itemToDrop);

    Item* getDroppingItem();

    std::unique_ptr<Item> takeDroppingItem();

protected:
    int      entityId;
    uint32_t width   = 0;
    uint32_t height  = 0;
    bool     visible = true;
    bool     solid_  = false;
    std::unique_ptr<Item> itemToDrop_ = nullptr;

     uint32_t texX_ = 0;
     uint32_t texY_ = 0;

     bool flipH_ = false;
     bool flipV_ = false;
     bool flipD_ = false;

 private:
     ImVec2 pos{0.0f, 0.0f};
 };
