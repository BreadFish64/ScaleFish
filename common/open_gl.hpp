#pragma once

#include <iostream>
#include <optional>
#include <unordered_map>
#include <vector>

#include <fmt/ostream.h>

#include <glad/glad.h>

#include "sdl_helper.hpp"

extern "C" {
_declspec(dllexport) unsigned long NvOptimusEnablement = true;
}

namespace OpenGL {

#define GL_RESOURCE_WRAPPER(_resource)                                                             \
    struct _resource {                                                                             \
        GLuint handle = NULL;                                                                      \
                                                                                                   \
        _resource(_resource&& o) noexcept : handle(std::exchange(o.handle, 0)) {}                  \
                                                                                                   \
        _resource(bool create = false) {                                                           \
            if (create)                                                                            \
                Create();                                                                          \
        }                                                                                          \
                                                                                                   \
        void Create() {                                                                            \
            assert(!handle);                                                                       \
            glGen##_resource##s(1, &handle);                                                       \
        }                                                                                          \
                                                                                                   \
        ~_resource() {                                                                             \
            glDelete##_resource##s(1, &handle);                                                    \
        }                                                                                          \
                                                                                                   \
        operator GLuint() const {                                                                  \
            return handle;                                                                         \
        }                                                                                          \
    }

GL_RESOURCE_WRAPPER(Buffer);
GL_RESOURCE_WRAPPER(Framebuffer);
GL_RESOURCE_WRAPPER(Texture);
GL_RESOURCE_WRAPPER(VertexArray);
GL_RESOURCE_WRAPPER(Sampler);

#undef GL_RESOURCE_WRAPPER

struct Shader {
    GLuint handle = NULL;

    Shader(Shader&& o) noexcept : handle(std::exchange(o.handle, 0)) {}

    Shader(std::string_view source, GLenum shader_type) {
        Create(std::move(source), std::move(shader_type));
    }

    bool Create(std::string_view source, GLenum shader_type) {
        handle = glCreateShader(shader_type);
        const GLchar* const source_ptr = source.data();
        glShaderSource(handle, 1, &source_ptr, NULL);
        glCompileShader(handle);
        GLint isCompiled = 0;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint length = 0;
            glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);
            std::basic_string<GLchar> error_log(length, ' ');
            glGetShaderInfoLog(handle, length, &length, error_log.data());
            fmt::print(std::cerr, "Shader Compilation Failed\n{}\n", error_log);
            glDeleteShader(handle);
            handle = NULL;
        }
        return handle != NULL;
    }

    ~Shader() {
        if (handle)
            glDeleteShader(handle);
    }

    operator GLuint() const {
        return handle;
    }
};

struct Program {
    GLuint handle = NULL;

    Program(Program&& o) noexcept : handle(std::exchange(o.handle, 0)) {}

    Program(bool create = false) {
        if (create)
            Create();
    }

    Program(std::initializer_list<std::reference_wrapper<Shader>> shaders) {
        Create(std::move(shaders));
    }

    void Create() {
        handle = glCreateProgram();
    }

    bool Create(std::initializer_list<std::reference_wrapper<Shader>> shaders) {
        handle = glCreateProgram();
        for (const auto& shader : shaders) {
            glAttachShader(handle, shader.get());
        }
        glLinkProgram(handle);

        GLint isLinked = 0;
        glGetProgramiv(handle, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE) {
            GLint length = 0;
            glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);
            std::basic_string<GLchar> error_log(length, ' ');
            glGetProgramInfoLog(handle, length, &length, error_log.data());
            fmt::print(std::cerr, "Shader Linking Failed\n{}\n", error_log);
            glDeleteProgram(handle);
            handle = NULL;
        }

        for (const auto& shader : shaders) {
            glDetachShader(handle, shader.get());
        }
        return handle != NULL;
    }

    ~Program() {
        if (handle)
            glDeleteProgram(handle);
    }

    operator GLuint() const {
        return handle;
    }
};

static void APIENTRY DebugHandler(GLenum source, GLenum type, GLuint id, GLenum severity,
                                  GLsizei length, const GLchar* message, const void* user_param) {
    static const std::unordered_map<GLenum, std::string_view> severity_map{
        {GL_DEBUG_SEVERITY_NOTIFICATION, "Notification"},
        {GL_DEBUG_SEVERITY_LOW, "Info"},
        {GL_DEBUG_SEVERITY_MEDIUM, "Warning"},
        {GL_DEBUG_SEVERITY_HIGH, "Error"},
    };
    fmt::print("OpenGL {} {}: {}\n", severity_map.at(severity), id, message);
}

std::optional<SDL::GLContext> context;
SDL::SDLPtr<SDL_Window> fake_window;

void InitOffscreenContext() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    fake_window.reset(SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0,
                                       SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN));

    context.emplace(fake_window.get());

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        fmt::print(std::cerr, "OpenGL failed to load");
        std::exit(2);
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(DebugHandler, nullptr);

    return;
}

} // namespace OpenGL