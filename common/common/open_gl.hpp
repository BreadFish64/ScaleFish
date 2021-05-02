#pragma once

#include "sdl_helper.hpp"

#include <fmt/ostream.h>

#include <glad/glad.h>

#include <cassert>
#include <iostream>

namespace OpenGL {
namespace Resource {
#define GL_RESOURCE_WRAPPER(_resource)                                                             \
    struct _resource {                                                                             \
        GLuint handle = NULL;                                                                      \
                                                                                                   \
        _resource(_resource&& o) noexcept : handle(std::exchange(o.handle, 0)) {}                  \
                                                                                                   \
        _resource(bool create = false) {                                                           \
            if (create) Create();                                                                  \
        }                                                                                          \
                                                                                                   \
        void Create() {                                                                            \
            assert(!handle);                                                                       \
            glGen##_resource##s(1, &handle);                                                       \
        }                                                                                          \
                                                                                                   \
        ~_resource() { glDelete##_resource##s(1, &handle); }                                       \
                                                                                                   \
        operator GLuint() const { return handle; }                                                 \
    }

GL_RESOURCE_WRAPPER(Buffer);
GL_RESOURCE_WRAPPER(Framebuffer);
GL_RESOURCE_WRAPPER(Texture);
GL_RESOURCE_WRAPPER(VertexArray);
GL_RESOURCE_WRAPPER(Sampler);
GL_RESOURCE_WRAPPER(Renderbuffer);
GL_RESOURCE_WRAPPER(Querie);
#undef GL_RESOURCE_WRAPPER

} // namespace Resource

using Resource::Buffer;
using Resource::Framebuffer;
using Resource::Renderbuffer;
using Resource::Sampler;
using Resource::Texture;
using Resource::VertexArray;
using Query = Resource::Querie;

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
        if (handle) glDeleteShader(handle);
    }

    operator GLuint() const { return handle; }
};

struct Program {
    GLuint handle = NULL;

    Program(Program&& o) noexcept : handle(std::exchange(o.handle, 0)) {}

    Program(bool create = false) {
        if (create) Create();
    }

    Program(std::initializer_list<std::reference_wrapper<Shader>> shaders) {
        Create(std::move(shaders));
    }

    void Create() { handle = glCreateProgram(); }

    bool Create(std::initializer_list<std::reference_wrapper<Shader>> shaders) {
        handle = glCreateProgram();
        for (const auto& shader : shaders) { glAttachShader(handle, shader.get()); }
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

        for (const auto& shader : shaders) { glDetachShader(handle, shader.get()); }
        return handle != NULL;
    }

    ~Program() {
        if (handle) glDeleteProgram(handle);
    }

    operator GLuint() const { return handle; }
};

std::pair<SDLPtr<SDL_Window>, SDLPtr<std::remove_pointer_t<SDL_GLContext>>> InitContext(
    bool offscreen, bool debug_opengl);

} // namespace OpenGL