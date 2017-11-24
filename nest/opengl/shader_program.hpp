#pragma once

#include <algorithm>
#include <string_view>

#include <GL/glew.h>

namespace nest {
inline namespace v1 {

/// A class for managing OpenGL shader program objects.
class ShaderProgram final {
  public:
    class Builder;

    ShaderProgram() noexcept = default;

    ShaderProgram(ShaderProgram const&) = delete;
    ShaderProgram(ShaderProgram&& that)
    {
        swap(that);
    }

    ~ShaderProgram() noexcept
    {
        if (handle) {
            glDeleteProgram(handle);
        }
    }

    ShaderProgram& operator=(ShaderProgram const&) = delete;
    ShaderProgram& operator=(ShaderProgram&& that)
    {
        swap(that);
        return *this;
    }

    void swap(ShaderProgram& that) noexcept
    {
        std::swap(handle, that.handle);
    }

    /// Checks whether the shader program is valid.
    explicit operator bool() const
    {
        return 0u != handle;
    }

    /// Makes the shader program be the one that will be used for drawing.
    void enable()
    {
        if (handle) {
            glUseProgram(handle);
        }
    }

  private:
    GLuint handle = 0u;
};

namespace detail {

/// \returns A handle to the shader of the given `type`, compiled with the given `source` code, or `nullptr` in case of
/// compilation error.
GLuint compile_shader(GLenum type, std::string_view const& source)
{
    auto handle = glCreateShader(type);
    if (!handle) {
        // TODO: report an error.
    }
    else {
        // clang-format off
        GLchar const* source_data[] = { source.data() };
        GLint  const  source_size[] = { static_cast<GLint>(source.size()) };
        // clang-format on
        glShaderSource(handle, 1, source_data, source_size);
        glCompileShader(handle);

        GLint status = GL_FALSE;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
        if (GL_FALSE == status) {

            std::array<char, 1024u> buffer;
            glGetShaderInfoLog(handle, buffer.size(), nullptr, buffer.data());

            // TODO: report an error.

            glDeleteShader(handle);
            handle = 0u;
        }
    }
    return handle;
}

/// Compiles and attaches a shader of the given `type` with the given `source` code to a shader program with the given
/// `program_handle`.
void compile_and_attach(GLuint program_handle, GLenum type, std::string_view const& source)
{
    if (source.empty())
        return;

    if (auto shader_handle = compile_shader(type, source.data()); shader_handle) {
        glAttachShader(program_handle, shader_handle);
        glDeleteShader(shader_handle);
    }
    else {
        // TODO: report an error.
    }
}

} // namespace detail

/// A class for building instances of `ShaderProgram`.
class ShaderProgram::Builder final {
  public:
    /// Compiles a vertex shader with the given `source`. The shader will be linked into the `ShaderProgram` under
    /// construction.
    Builder& with_vertex_shader(std::string_view const& source)
    {
        if (lazy_init()) {
            detail::compile_and_attach(instance.handle, GL_VERTEX_SHADER, source);
        }
        return *this;
    }

    /// Compiles a fragment shader with the given `source`. The shader will be linked into the `ShaderProgram` under
    /// construction.
    Builder& with_fragment_shader(std::string_view const& source)
    {
        if (lazy_init()) {
            detail::compile_and_attach(instance.handle, GL_FRAGMENT_SHADER, source);
        }
        return *this;
    }

    /// \returns The built `ShaderProgram` instance.
    operator ShaderProgram()
    {
        if (lazy_init()) {
            glLinkProgram(instance.handle);

            GLint status = GL_FALSE;
            glGetProgramiv(instance.handle, GL_LINK_STATUS, &status);
            if (GL_FALSE == status) {

                std::array<char, 1024u> buffer;
                glGetProgramInfoLog(instance.handle, buffer.size(), nullptr, buffer.data());

                // TODO: report the error.

                glDeleteProgram(instance.handle);
                instance.handle = 0u;
            }
        }
        return std::move(instance);
    }

  private:
    /// Creates the shader program object unless it has been created already.
    bool lazy_init()
    {
        if (!instance.handle) {
            if (auto handle = glCreateProgram(); handle) {
                instance.handle = handle;
            }
            else {
                // TODO: report the error.
                return false;
            }
        }
        return true;
    }

    /// Holds the `ShaderProgram` instance being built.
    ShaderProgram instance;
};

} // namespace v1
} // namespace nest
