#include <memory>

#define SDL_MAIN_HANDLED
#include <SDL.h>

namespace SDL {

struct GLContext {
    SDL_GLContext ctx;
    explicit GLContext(SDL_Window* window) {
        ctx = SDL_GL_CreateContext(window);
    }
    ~GLContext() {
        SDL_GL_DeleteContext(ctx);
    }
};

struct SDL_Deleter {
    void operator()(SDL_Window* p) const;
    void operator()(SDL_Renderer* p) const;
    void operator()(SDL_Texture* p) const;
    void operator()(SDL_GLContext* p) const;
};

void SDL_Deleter::operator()(SDL_Window* p) const {
    SDL_DestroyWindow(p);
}

void SDL_Deleter::operator()(SDL_Renderer* p) const {
    SDL_DestroyRenderer(p);
}

void SDL_Deleter::operator()(SDL_Texture* p) const {
    SDL_DestroyTexture(p);
}

void SDL_Deleter::operator()(SDL_GLContext* p) const {
    SDL_GL_DeleteContext(p);
}

template <typename T>
using SDLPtr = std::unique_ptr<T, SDL_Deleter>;

} // namespace SDL