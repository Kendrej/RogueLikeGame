// Minimalny stub vk_types.h — tylko najprostsze typy, by kompilacja przeszła bez Vulkan SDK.
#pragma once

// W prawdziwym projekcie tutaj byłyby definicje typów Vulkan (VkInstance, VkDevice itp.).
using VkInstance = void*;
using VkDevice = void*;

struct VkExtent2D { uint32_t width; uint32_t height; };

