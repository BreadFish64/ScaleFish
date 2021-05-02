#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <memory>

struct SDL_Deleter {
    void operator()(SDL_Window* p) const;
    void operator()(SDL_Renderer* p) const;
    void operator()(SDL_Texture* p) const;
    void operator()(SDL_GLContext p) const;
};

template <typename T>
using SDLPtr = std::unique_ptr<T, SDL_Deleter>;
