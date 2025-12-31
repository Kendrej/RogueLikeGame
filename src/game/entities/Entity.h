#pragma once
#include <cstdint>
#include <imgui.h>
#include <memory>


enum class ItemId : int;
class Assets;
class Item;

enum class EntityType {
    Default, Item, Player, Npc, Projectile, Tile
};

class Entity
{
public:
    Entity(int entityId, uint32_t width, uint32_t height, float pos_x, float pos_y, bool solid = false, EntityType type = EntityType::Default);
    virtual ~Entity();

    ImVec2   getPosition() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    void setSize(uint32_t w, uint32_t h);
    int      getEntityId() const;
    void     setEntityId(int id);
    bool     isVisible() const;
    bool isLockedDoorHelper() const
    {
        return lockedDoorHelper_;
    }
    void setLockedDoorHelper(bool val)
    {
        lockedDoorHelper_ = val;
    }
    void setVisible(bool visible);
    void setPosition(float x, float y);

    virtual void update(float /*dt*/) {}
    void         moveBy(float dx, float dy);

    EntityType     getEntityType() const {return type_ ;}

    bool isItem() const { return type_ == EntityType::Item ;}
    bool isNpc() const { return type_ == EntityType::Npc ;}


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
    void  setGivingItem(ItemId item);

    Item* getDroppingItem();
    ItemId getGivingItem();

    std::unique_ptr<Item> takeDroppingItem();

protected:
    int      entityId;
    EntityType type_ = EntityType::Default;
    uint32_t width   = 0;
    uint32_t height  = 0;
    bool     visible = true;
    bool     solid_  = false;
    std::unique_ptr<Item> itemToDrop_ = nullptr;
    ItemId itemTogiveId_;
    bool lockedDoorHelper_ = false;
     uint32_t texX_ = 0;
     uint32_t texY_ = 0;

     bool flipH_ = false;
     bool flipV_ = false;
     bool flipD_ = false;

 private:
     ImVec2 pos{0.0f, 0.0f};
 };
