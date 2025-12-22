#pragma once
#include <cstdint>
#include <imgui.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

using IconId = int;

// GPU-side of an icon/texture (no position/visibility - that is game logic)
struct IconGPU
{
    VkImage        image   = VK_NULL_HANDLE;
    VkDeviceMemory memory  = VK_NULL_HANDLE;
    VkImageView    view    = VK_NULL_HANDLE;
    VkSampler      sampler = VK_NULL_HANDLE;
    ImTextureID    imTex   = (ImTextureID)0;
    uint32_t       width   = 0;
    uint32_t       height  = 0;
};

class Assets
{
public:
    struct Ctx
    {
        VkPhysicalDevice physicalDevice{};
        VkDevice         device{};
        VkQueue          graphicsQueue{};
        VkCommandPool    commandPool{};
    };

    explicit Assets(const Ctx& ctx);
    ~Assets();

    void setCommandPool(VkCommandPool pool);

    IconId addIconFromFile(const std::string& path);
    IconId getOrLoadIcon(const std::string& path);

    // Load sprite sheet and split into frames
    // Returns base IconId (ID of first frame)
    IconId loadSpriteSheet(const std::string& path, int frameCount, int frameWidth, int frameHeight);

    const IconGPU& icon(IconId id) const
    {
        return icons_[id];
    }

    void removeIcon(IconId id); // leaves a hole - stable ID
    void clear();               // clears everything

private:
    Ctx                  ctx_;
    std::vector<IconGPU> icons_;

    std::unordered_map<std::string, IconId> byPath_;
    std::vector<std::string>                paths_;

    // Helpers (moved from Texture.cpp)
    uint32_t        findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
    VkCommandBuffer beginSingleTimeCommands() const;
    void            endSingleTimeCommands(VkCommandBuffer cmd) const;
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer,
                      VkDeviceMemory& bufferMemory) const;
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const;
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const;
    VkImageView createImageView(VkImage image, VkFormat format) const;

    // Helper to create icon from pixel data
    IconId createIconFromPixels(const unsigned char* pixels, int width, int height, const std::string& cacheName);

    static void destroyIcon(const Ctx& ctx, IconGPU& s);
};
