#pragma once

#include <nest/config.hpp>

#if NEST_RENDERER == NEST_RENDERER_OPENGL
#include <nest/opengl/context.hpp>
#endif

namespace nest {
inline namespace v1 {

#if NEST_RENDERER == NEST_RENDERER_OPENGL

/// Creates default OpenGL context with the following attributes:
///     - RGBA color buffer, 8-bits per channel;
///     - 24-bit depth and 8-bit stencil buffers;
///     - OpenGL 4.1 core profile;
///     - Enabled double buffering.
OpenGL make_renderer_context(std::string const& title, int width, int height)
{
    return OpenGL::Builder{}
        .with_window(title, width, height)
        .with_color(8, 8, 8, 8)
        .with_depth_and_stencil(24, 8)
        .with_version(4, 1)
        .with(nest::OpenGL::Profile::Core)
        .with(nest::OpenGL::DoubleBuffering::On);
}

#endif

} // namespace v1
} // namespace nest
