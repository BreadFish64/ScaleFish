#include "open_gl.hpp"

extern "C" {
_declspec(dllexport) unsigned long NvOptimusEnablement = true;
}

namespace OpenGL {

static void APIENTRY DebugHandler(GLenum source, GLenum type, GLuint id, GLenum severity,
                                  GLsizei length, const GLchar* message, const void* user_param) {
    std::string_view severity_string;
    switch (severity) {
    case GL_DEBUG_SEVERITY_NOTIFICATION: severity_string = "Notification";
    case GL_DEBUG_SEVERITY_LOW: severity_string = "Info";
    case GL_DEBUG_SEVERITY_MEDIUM: severity_string = "Warning";
    case GL_DEBUG_SEVERITY_HIGH: severity_string = "Error";
    default: severity_string = "";
    };
    fmt::print("OpenGL {} {}: {}\n", severity_string, id, message);
}

std::pair<SDLPtr<SDL_Window>, SDLPtr<std::remove_pointer_t<SDL_GLContext>>> InitContext(
    bool offscreen, bool debug_opengl) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    if (debug_opengl) SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    SDLPtr<SDL_Window> window{
        SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0,
                         SDL_WINDOW_OPENGL | (SDL_WINDOW_HIDDEN * offscreen))};
    SDLPtr<std::remove_pointer_t<SDL_GLContext>> context{SDL_GL_CreateContext(window.get())};

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        fmt::print(std::cerr, "OpenGL failed to load");
        std::exit(2);
    }

    if (debug_opengl) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(DebugHandler, nullptr);
    }

    return std::make_pair(std::move(window), std::move(context));
}

} // namespace OpenGL