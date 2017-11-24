#pragma once

#include <algorithm>
#include <initializer_list>
#include <iterator>

#include <GL/glew.h>

#include <nest/vertex_traits.hpp>

namespace nest {
inline namespace v1 {

///
class Mesh final {
  public:
    class Builder;

    /// Constructs an empty `Mesh`.
    Mesh() noexcept = default;

    Mesh(Mesh const&) = delete;
    Mesh(Mesh&& that) noexcept
    {
        swap(that);
    }

    ~Mesh() noexcept
    {
        // A value of 0 will be silently ignored.
        glDeleteVertexArrays(1, &vao_handle);

        // A value of 0 will be silently ignored.
        glDeleteBuffers(VboCount, vbo_handle);
    }

    Mesh& operator=(Mesh const&) = delete;
    Mesh& operator=(Mesh&& that) noexcept
    {
        swap(that);
        return *this;
    }

    void swap(Mesh& that)
    {
        std::swap(vao_handle, that.vao_handle);
        std::swap(vbo_handle, that.vbo_handle);
    }

    /// \returns `true` when this `Mesh` is not empty, `false` otherwise.
    explicit operator bool() const
    {
        return 0u != vao_handle;
    }

    /// Makes the `Mesh` be used for drawing.
    void enable()
    {
        if (vao_handle) {
            glBindVertexArray(vao_handle);
        }
    }

  private:
    enum { Vertices, Indices, VboCount };

    GLuint vao_handle = 0u;
    GLuint vbo_handle[VboCount] = {0u, 0u};
};

namespace detail {

/// Lazily creates a VBO and initializes it with the given data.
template <typename T> // T models RandomAccessIterator
bool set_buffer_data(GLenum const target, GLuint& vbo, T begin, T end, GLenum usage = GL_STATIC_DRAW)
{
    using Item = typename std::iterator_traits<T>::value_type;

    auto const num_items = end - begin;
    if (!num_items) {
        // TODO: create an empty VBO maybe?
        return false;
    }

    if (!vbo) {
        glGenBuffers(1, &vbo);
        if (!vbo) {
            // TODO: report the error.
            return false;
        }
    }

    glBindBuffer(target, vbo);
    glBufferData(target, num_items * sizeof(Item), &begin[0], usage);

    return true;
}

///
template <typename T>
void enable_attribute(GLuint const index, GLboolean const normalized, GLvoid const* pointer)
{
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, component_count<T>, opengl_type<T>, normalized, sizeof(T), pointer);
}

} // namespace detail

/// A class template for building instances of `Mesh` class.
class Mesh::Builder final {
  public:
    /// Sets vertices of the `Mesh` being built.
    template <typename T> // T models RandomAccessIterator
    Builder& with_vertices(T begin, T end)
    {
        if (lazy_init() && detail::set_buffer_data(GL_ARRAY_BUFFER, instance.vbo_handle[Vertices], begin, end)) {
            using Vertex = typename std::iterator_traits<T>::value_type;

            if constexpr (has_position<Vertex>) {
                detail::enable_attribute<decltype(Vertex::position)>(PositionAttribute, GL_FALSE,
                                                                     offsetof(Vertex, position));
            }

            if constexpr (has_color<Vertex>) {
                detail::enable_attribute<decltype(Vertex::color)>(ColorAttribute, GL_FALSE, offsetof(Vertex, color));
            }

            if constexpr (has_texcoord<Vertex>) {
                detail::enable_attribute<decltype(Vertex::texcoord)>(TexcoordAttribute, GL_FALSE,
                                                                     offsetof(Vertex, texcoord));
            }
        }
        else {
            // TODO: report the error.
        }
        return *this;
    }

    /// Sets indices of the `Mesh` being built.
    template <typename T> // T models RandomAccessIterator
    Builder& with_indices(T begin, T end)
    {
        if (lazy_init() && detail::set_buffer_data(GL_ELEMENT_ARRAY_BUFFER, instance.vbo_handle[Indices], begin, end)) {
        }
        else {
            // TODO: report the error.
        }
        return *this;
    }

    /// Sets vertices with the given `container`.
    template <typename T> // T models ContiguousContainer
    Builder& with_vertices(T const& container)
    {
        using std::begin, std::end;
        return with_vertices(begin(container), end(container));
    }

    /// Sets indices with the given `container`.
    template <typename T> // T models ContiguousContainer
    Builder& with_indices(T const& container)
    {
        using std::begin, std::end;
        return with_vertices(begin(container), end(container));
    }

    ///
    template <typename T>
    Builder& with_vertices(std::initializer_list<T> vertices)
    {
        using std::begin, std::end;
        return with_vertices(begin(vertices), end(vertices));
    }

    ///
    template <typename T>
    Builder& with_indices(std::initializer_list<T> indices)
    {
        using std::begin, std::end;
        return with_indices(begin(indices), end(indices));
    }

    /// \returns The built `Mesh` instance.
    operator Mesh()
    {
        return std::move(instance);
    }

  private:
    ///
    bool lazy_init()
    {
        auto& vao = instance.vao_handle;
        if (!vao) {
            glGenVertexArrays(1, &vao);
            if (!vao) {
                // TODO: report the error.
                return false;
            }
        }

        glBindVertexArray(vao);
        return true;
    }

    ///
    Mesh instance;
};

} // namespace v1
} // namespace nest
