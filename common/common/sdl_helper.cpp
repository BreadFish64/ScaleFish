#include "sdl_helper.hpp"

void SDL_Deleter::operator()(SDL_Window* p) const { SDL_DestroyWindow(p); }

void SDL_Deleter::operator()(SDL_Renderer* p) const { SDL_DestroyRenderer(p); }

void SDL_Deleter::operator()(SDL_Texture* p) const { SDL_DestroyTexture(p); }

void SDL_Deleter::operator()(SDL_GLContext p) const { SDL_GL_DeleteContext(p); }