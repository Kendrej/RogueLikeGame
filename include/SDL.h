// Minimalny stub SDL.h używany wyłącznie by skompilować przykład bez zależności zewnętrznych.
#pragma once

#include <cstdint>

using Uint32 = uint32_t;

// Flags dla SDL_Init
constexpr Uint32 SDL_INIT_VIDEO = 0x00000020u;

// Event types
constexpr int SDL_QUIT = 0x100;
constexpr int SDL_WINDOWEVENT = 0x200;

// Window events
constexpr int SDL_WINDOWEVENT_MINIMIZED = 1;
constexpr int SDL_WINDOWEVENT_RESTORED = 2;

// Window flags
using SDL_WindowFlags = unsigned int;
constexpr SDL_WindowFlags SDL_WINDOW_VULKAN = 0x00000001u;

// Window position
constexpr int SDL_WINDOWPOS_UNDEFINED = -1;

// Minimal forward-declare of SDL_Window
struct SDL_Window {
    int dummy = 0;
};

// Minimal SDL_Event used by vk_engine.cpp
struct SDL_Event {
    int type = 0;
    struct { int event = 0; } window;
};

// Minimal implementations as inline functions so linkowanie nie jest potrzebne.
inline int SDL_Init(Uint32 /*flags*/) { return 0; }
inline SDL_Window* SDL_CreateWindow(const char* /*title*/, int /*x*/, int /*y*/, int /*w*/, int /*h*/, SDL_WindowFlags /*flags*/) {
    return new SDL_Window();
}
inline void SDL_DestroyWindow(SDL_Window* w) { delete w; }
// Tymczasowy testowy SDL_PollEvent: zwraca pojedynczy event SDL_QUIT przy pierwszym wywołaniu,
// potem zawsze 0. Ułatwia szybkie zakończenie programu podczas testów lokalnych.
inline int SDL_PollEvent(SDL_Event* e) {
    static bool returned_quit = false;
    if (!returned_quit) {
        returned_quit = true;
        if (e) {
            e->type = SDL_QUIT;
            e->window.event = 0;
        }
        return 1;
    }
    return 0;
}
