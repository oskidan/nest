#pragma once

//
// Supported renderers:
//

#define NEST_RENDERER_OPENGL 0

// Make OpenGL the default renderer.
#ifndef NEST_RENDERER
#define NEST_RENDERER NEST_RENDERER_OPENGL
#endif
