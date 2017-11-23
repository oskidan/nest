#pragma once

#include <type_traits>
#include <utility>

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

} // namespace v1
} // namespace nest

