#pragma once

#include <algorithm>

#include <SDL2/SDL.h>

namespace nest {
inline namespace v1 {

/// A class for managing OpenGL context. The instances of this class are immutable.
class OpenGL final {
  public:
    /// Enumerates possible OpenGL profiles, the actual availability depends on your target platform.
    enum class Profile { Core, Compatability, ES };

    /// Enumerates possible states of doulbe buffering.
    enum class DoubleBuffering { On, Off };

    class Builder;

    OpenGL() noexcept = default;

    /// Performs OpenGL shutdown.
    ~OpenGL() noexcept
    {
        if (context) {
            SDL_GL_DeleteContext(context);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
    };

    OpenGL(OpenGL const& that) = delete;

    OpenGL(OpenGL&& that) noexcept
    {
        swap(that);
    }

    OpenGL& operator=(OpenGL const& that) = delete;

    OpenGL& operator=(OpenGL&& that) noexcept
    {
        swap(that);

        return *this;
    }

    /// Swaps contents of this `OpenGL` object with `that` one.
    void swap(OpenGL& that) noexcept
    {
        // clang-format off
        std::swap(window,  that.window);
        std::swap(context, that.context);
        // clang-format on
    }

    /// Makes the context, manged by this `OpenGL` object, be the current one.
    void make_current()
    {
        SDL_GL_MakeCurrent(window, context);
    }

    /// Swaps front and back buffers, updating the window with OpenGL rendering.
    void swap_buffers()
    {
        SDL_GL_SwapWindow(window);
    }

    /// Checks whether the `OpenGL` manages a non-null context.
    explicit operator bool() const
    {
        return nullptr != context;
    }

  private:
    /// Holds a pointer to the SDL window to be drawn to.
    SDL_Window* window = nullptr;

    /// Holds an opaque poitner to SDL OpenGL context object.
    SDL_GLContext context = nullptr;
};

/// A class for making `OpenGL` instances.
class OpenGL::Builder final {
  public:
    /// Specifies the title and dimensions of OpenGL window.
    Builder& with_window(std::string title, int width, int height)
    {
        window_title = title;
        window_width = width;
        window_height = height;

        return *this;
    }

    /// Specifies the minimum number of bits per color buffer channels.
    Builder& with_color(int num_r_bits, int num_g_bits, int num_b_bits, int num_a_bits)
    {
        // clang-format off
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   num_r_bits);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, num_g_bits);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  num_b_bits);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, num_a_bits);
        // clang-format on

        return *this;
    }

    /// Specifies the minimum number of bits in the depth and stencil buffers.
    Builder& with_depth_and_stencil(int num_depth_bits, int num_stencil_bits)
    {
        // clang-format off
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,   num_depth_bits);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, num_stencil_bits);
        // clang-format on

        return *this;
    }

    /// Specifies whether the output is single or doulbe buffered.
    Builder& with(DoubleBuffering const double_buffering)
    {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, DoubleBuffering::On == double_buffering ? 1 : 0);

        return *this;
    }

    /// Specifies which version of OpenGL must be supported.
    Builder& with_version(int major, int minor)
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);

        return *this;
    }

    /// Specifies which OpenGL profile must be supported by the context.
    Builder& with(Profile const profile)
    {
        int value =
            (Profile::Core == profile ? SDL_GL_CONTEXT_PROFILE_CORE
                                      : (Profile::Compatability == profile ? SDL_GL_CONTEXT_PROFILE_COMPATIBILITY
                                                                           : SDL_GL_CONTEXT_PROFILE_ES));
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, value);

        return *this;
    }

    /// \returns The built `OpenGL` instance.
    operator OpenGL()
    {
        auto const title = window_title.empty() ? "" : window_title.data();

        auto const x = SDL_WINDOWPOS_CENTERED;
        auto const y = SDL_WINDOWPOS_CENTERED;

        // TODO: Seems like `SDL_WINDOW_RESIZABLE` is a good candidate for being configured though the builder as well.
        instance.window =
            SDL_CreateWindow(title, x, y, window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

        if (!instance.window) {
            // TODO: Output error notification.
        }
        else {
            instance.context = SDL_GL_CreateContext(instance.window);
            if (!instance.context) {
                // TODO: Output error notification.
            }
        }

        return std::move(instance);
    }

  private:
    /// Holds the title of the window being built.
    std::string window_title = "";

    // clang-format off
    /// Holds the size of the window being built.
    /// @{
    int window_width  = 800;
    int window_height = 600;
    /// @}
    // clang-format on

    /// Holds the `OpenGL` instance being built.
    OpenGL instance;
};

} // namespace v1
} // namespace nest
