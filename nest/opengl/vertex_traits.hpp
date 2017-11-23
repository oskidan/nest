#pragma once

#include <cstddef>

#include <GL/glew.h>

namespace nest {
inline namespace v1 {

/// Holds a `GLenum` that corresponds to the given type `T`.
///@{
// clang-format off

template <typename T> static constexpr GLenum opengl_type;

// 8-bit, singed, 2's complement binary integer:
template <>              constexpr GLenum opengl_type<GLbyte>      = GL_BYTE;
template <std::size_t N> constexpr GLenum opengl_type<GLbyte[N]>   = GL_BYTE;

// 8-bit, unsigned binary integer:
template <>              constexpr GLenum opengl_type<GLubyte>     = GL_UNSIGNED_BYTE;
template <std::size_t N> constexpr GLenum opengl_type<GLubyte[N]>  = GL_UNSIGNED_BYTE;

// 16-bit, signed, 2's complement binary integer:
template <>              constexpr GLenum opengl_type<GLshort>     = GL_SHORT;
template <std::size_t N> constexpr GLenum opengl_type<GLshort[N]>  = GL_SHORT;

// 16-bit, unsigned binary integer:
template <>              constexpr GLenum opengl_type<GLushort>    = GL_UNSIGNED_SHORT;
template <std::size_t N> constexpr GLenum opengl_type<GLushort[N]> = GL_UNSIGNED_SHORT;

// 32-bit, signed, 2's complement binary integer:
template <>              constexpr GLenum opengl_type<GLint>       = GL_INT;
template <std::size_t N> constexpr GLenum opengl_type<GLint[N]>    = GL_INT;
template <>              constexpr GLenum opengl_type<glm::ivec2>  = GL_INT;
template <>              constexpr GLenum opengl_type<glm::ivec3>  = GL_INT;
template <>              constexpr GLenum opengl_type<glm::ivec4>  = GL_INT;

// 32-bit, unsigned binary integer:
template <>              constexpr GLenum opengl_type<GLuint>      = GL_UNSIGNED_INT;
template <std::size_t N> constexpr GLenum opengl_type<GLuint[N]>   = GL_UNSIGNED_INT;
template <>              constexpr GLenum opengl_type<glm::uvec2>  = GL_UNSIGNED_INT;
template <>              constexpr GLenum opengl_type<glm::uvec3>  = GL_UNSIGNED_INT;
template <>              constexpr GLenum opengl_type<glm::uvec4>  = GL_UNSIGNED_INT;

// TODO(oleksii): Implement strongly-typed type alias for `GLhalf`, since it's a typedef of `unsigned int` just like `GLushort`.
#if 0
// 16-bit, IEEE-754 floating-point value:
template <>              constexpr GLenum opengl_type<GLhalf>      = GL_HALF_FLOAT;
template <std::size_t N> constexpr GLenum opengl_type<GLhalf[N]>   = GL_HALF_FLOAT;
#endif

// 32-bit, IEEE-754 floating-point value:
template <>              constexpr GLenum opengl_type<GLfloat>     = GL_FLOAT;
template <std::size_t N> constexpr GLenum opengl_type<GLfloat[N]>  = GL_FLOAT;
template <>              constexpr GLenum opengl_type<glm::vec2>   = GL_FLOAT;
template <>              constexpr GLenum opengl_type<glm::vec3>   = GL_FLOAT;
template <>              constexpr GLenum opengl_type<glm::vec4>   = GL_FLOAT;

// 64-bit, IEEE-754 floating-point value:
template <>              constexpr GLenum opengl_type<GLdouble>    = GL_DOUBLE;
template <std::size_t N> constexpr GLenum opengl_type<GLdouble[N]> = GL_DOUBLE;
template <>              constexpr GLenum opengl_type<glm::dvec2>  = GL_DOUBLE;
template <>              constexpr GLenum opengl_type<glm::dvec3>  = GL_DOUBLE;
template <>              constexpr GLenum opengl_type<glm::dvec4>  = GL_DOUBLE;

// TODO(oleksii): Implement strongly-typed type alias for `GLfixed`, since it's a typedef of `int` just like `GLint`.
#if 0
// 32-bit, signed, 2's complement 16.16 integer:
template <>              constexpr GLenum opengl_type<GLfixed>     = GL_FIXED;
template <std::size_t N> constexpr GLenum opengl_type<GLfixed[N]>  = GL_FIXED;
#endif

// clang-format on
///@}

} // namespace v1
} // namespace nest

