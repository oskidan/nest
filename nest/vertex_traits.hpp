#pragma once

#include <type_traits>
#include <utility>

// REVISIT(oleksii): Figure out if there're going to be any issues with GLM when targeting DX.
#include <glm/glm.hpp>

#include <nest/config.hpp>

#if NEST_RENDERER == NEST_RENDERER_OPENGL
#include <nest/opengl/vertex_traits.hpp>
#endif

namespace nest {
inline namespace v1 {

/// Holds a boolean value which specifies whether the given type `T` has a field named `position`.
/// @{
template <typename T, typename = void>
static constexpr bool has_position = false;

template <typename T>
constexpr bool has_position<T, std::void_t<decltype(std::declval<T>().position)>> = true;
/// @}

/// Holds a boolean value which specifies whether the given type `T` has a field named `color`.
/// @{
template <typename T, typename = void>
static constexpr bool has_color = false;

template <typename T>
constexpr bool has_color<T, std::void_t<decltype(std::declval<T>().color)>> = true;
/// @}

/// Holds a boolean value which specifies whether the given type `T` has a field named `texcoord`.
/// @{
template <typename T, typename = void>
static constexpr bool has_texcoord = false;

template <typename T>
constexpr bool has_texcoord<T, std::void_t<decltype(std::declval<T>().texcoord)>> = true;
/// @}

/// Holds a numeric value which represents the number of components per vertex attribute.
/// @{
// clang-format off
template<typename T> static constexpr std::size_t component_count = 1u;

template<typename T, std::size_t N> constexpr std::size_t component_count<T[N]>       = N;
template<>                          constexpr std::size_t component_count<glm::vec2>  = 2u;
template<>                          constexpr std::size_t component_count<glm::vec3>  = 3u;
template<>                          constexpr std::size_t component_count<glm::vec4>  = 4u;
template<>                          constexpr std::size_t component_count<glm::dvec2> = 2u;
template<>                          constexpr std::size_t component_count<glm::dvec3> = 3u;
template<>                          constexpr std::size_t component_count<glm::dvec4> = 4u;
template<>                          constexpr std::size_t component_count<glm::ivec2> = 2u;
template<>                          constexpr std::size_t component_count<glm::ivec3> = 3u;
template<>                          constexpr std::size_t component_count<glm::ivec4> = 4u;
template<>                          constexpr std::size_t component_count<glm::uvec2> = 2u;
template<>                          constexpr std::size_t component_count<glm::uvec3> = 3u;
template<>                          constexpr std::size_t component_count<glm::uvec4> = 4u;
// clang-format on
/// @}

} // namespace v1
} // namespace nest
