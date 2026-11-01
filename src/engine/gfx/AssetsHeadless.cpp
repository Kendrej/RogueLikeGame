// Implementacja Assets dla symulacji bez grafiki: wydaje stabilne identyfikatory,
// nie wczytuje obrazow i nie dotyka Vulkana. Uzywana zamiast Assets.cpp w RogueLikeSim.
#include "engine/gfx/Assets.h"

Assets::Assets(const Ctx& ctx) : ctx_(ctx) {}

Assets::~Assets() = default;

void Assets::setCommandPool(VkCommandPool pool)
{
    ctx_.commandPool = pool;
}

IconId Assets::addIconFromFile(const std::string& path)
{
    return getOrLoadIcon(path);
}

IconId Assets::getOrLoadIcon(const std::string& path)
{
    auto it = byPath_.find(path);
    if (it != byPath_.end())
        return it->second;

    const IconId id = static_cast<IconId>(icons_.size());
    icons_.emplace_back();
    byPath_[path] = id;
    return id;
}

IconId Assets::loadSpriteSheet(const std::string& path, int frameCount, int, int)
{
    auto it = byPath_.find(path);
    if (it != byPath_.end())
        return it->second;

    const IconId base = static_cast<IconId>(icons_.size());
    icons_.resize(icons_.size() + frameCount);
    byPath_[path] = base;
    return base;
}

void Assets::removeIcon(IconId) {}

void Assets::clear()
{
    icons_.clear();
    byPath_.clear();
}
