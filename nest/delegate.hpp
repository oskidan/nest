#pragma once

namespace nest {
inline namespace v1 {

namespace detail {

/// Peform unsafe type casting which is based on unions. This is required for member-function to `void*` conversion.
template <typename T, typename U>
T unsafe_cast(U value)
{
    union {
        U u;
        T t;
    };
    u = value;
    return t;
}

} // namespace detail

template <typename T>
class Delegate;

/// A class template for delegating calls to free-standing functions and objects.
template <typename R, typename... T>
class Delegate<R(T...)> {
  public:
    ~Delegate() noexcept
    {
        unbind();
    }

    /// Binds this `Delegate` to the given `function`.
    void bind(R (*function)(T...))
    {
        unbind();

        // This function will be called by `Delegate::operator()`. It's responsible for calling the free-standing
        // function, which is available through `Delegate::function` pointer.
        dispatch = [](Delegate* self, T... args) -> R {

            auto function = reinterpret_cast<R (*)(T...)>(self->function);
            if (!function) {
                // TODO: report an error.
            }

            return function(std::forward<T>(args)...);
        };

        this->function = reinterpret_cast<void*>(function);
    }

    /// Binds this `Delegate` to the given C-style `function` which takes `user_data` as its first argument. The
    /// lifetime of `user_data` shall be greater than or equal to the lifetime of this `Delegate`. The `user_data`
    /// pointer can be set to `nullptr`, it's `function`'s responsibility to check whether the given `user_data` is
    /// null.
    void bind(R (*function)(void*, T...), void* user_data)
    {
        unbind();

        // This function will be called by `Delegate::operator()`. It's responsible for call the free-standing function
        // which is available through `Delegate::function` pointer. That function will be given `Delegate::object`
        // pointer in the first argument.
        dispatch = [](Delegate* self, T... args) -> R {

            auto function = reinterpret_cast<R (*)(void*, T...)>(self->function);
            if (!function) {
                // TODO: report an error.
            }

            return function(self->object, std::forward<T>(args)...);
        };

        // clang-format off
        this->function = reinterpret_cast<void*>(function);
        this->object   = user_data;
        // clang-format on
    }

    /// Binds this `Delegate` to the given `object` and its member `function`. The lifetime of `object` shall be greater
    /// than or equal to the lifetime of this `Delegate`.
    template <typename U>
    void bind(U& object, R (U::*function)(T...))
    {
        unbind();

        // This function will be caleed by `Delegate::operator()`. It's responsible for calling the member function,
        // which is available through `Delegate::function` pointer on the object which is available through
        // `Delegate::object` pointer.
        dispatch = [](Delegate* self, T... args) -> R {

            auto fn = detail::unsafe_cast<R (U::*)(T...)>(self->function);
            if (!fn) {
                // TODO: report an error.
            }

            auto object = static_cast<U*>(self->object);
            if (!object) {
                // TODO: report an error.
            }

            return (object->*fn)(std::forward<T>(args)...);
        };

        // clang-format off
        this->object   = &object;
        this->function = detail::unsafe_cast<void*>(function);
        // clang-format on
    }

    /// Binds this `Delegate` to the given `callable` object. The object shall implment `R operator()(T...)`.
    template <typename U>
    void bind(U&& callable)
    {
        unbind();

        // This function will be called by `Delegate::opreator()`. It's responsible for calling the `operator()` of
        // `callable` object which is available through `Delegate::object` pointer.
        dispatch = [](Delegate* self, T... args) -> R {

            auto callable = static_cast<U*>(self->object);
            if (!callable) {
                // TODO: report an error.
            }

            return (*callable)(std::forward<T>(args)...);
        };

        // This function will be called by `Delegate::unbind()`. It's responsible for gracefull destruction and
        // deallocation of `callable` object.
        destroy = [](Delegate* self) {

            auto callable = static_cast<U*>(self->object);
            if (!callable) {
                // TODO: report an error.
            }

            delete callable;
        };

        /// clang-format off
        this->object = new U(std::forward<U>(callable));
        this->destroy = destroy;
        /// clang-format on
    }

    /// Invokes the callee with the given arguments.
    template <typename... U>
    R operator()(U&&... args)
    {
        if (!dispatch) {
            // TODO: report an error.
        }

        return dispatch(this, std::forward<U>(args)...);
    }

    /// Unbinds this `Delegate` from the callee.
    void unbind()
    {
        if (destroy) {
            destroy(this);
        }

        // clang-format off
        object   = nullptr;
        function = nullptr;
        dispatch = nullptr;
        destroy  = nullptr;
        // clang-format on
    }

    /// Checks whether this `Delegate` is bound to anything.
    explicit operator bool() const
    {
        return nullptr != dispatch;
    }

  private:
    // TODO(oleksii): merge `destroy` and `function` into a single tagged pointer.

    /// If this delegate is bound to a member function, holds a pointer to the object which member function this
    /// delegate is bound to, otherwise holds `nullptr`.
    void* object = nullptr;

    /// If this delegate is bound to a member function, holds a pointer to the member function. If this delegate is
    /// bound to a free-standing function, holds a pointer to that function, otherwise holds `nullptr`.
    void* function = nullptr;

    /// Holds a pointer to a function that knows how to dispatch call to the callee. Holds `nullptr` if nothing has been
    /// bound to this delegate.
    R (*dispatch)(Delegate* self, T...) = nullptr;

    /// Holds a pointer to a function that knows how to destroy callable object. The callable object must be stored in
    /// `object` member variable of `Delegate` class.
    void (*destroy)(Delegate* self) = nullptr;
};

} // namespace v1
} // namespace nest

