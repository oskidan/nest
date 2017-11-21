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
    /// Binds this `Delegate` to the given `function`.
    void bind(R (*function)(T...))
    {
        // This function will be called by `Delegate::operator()`. It's responsible for calling the free-standing
        // function, which is available through `Delegate::function` pointer.
        dispatch = [](Delegate* self, T&&... args) -> R {

            auto function = reinterpret_cast<R (*)(T...)>(self->function);
            if (!function) {
                // TODO: report an error.
            }

            return function(std::forward<T>(args)...);
        };

        this->function = reinterpret_cast<void*>(function);
    }

    /// Binds this `Delegate` to the given `object` and its member `function`.
    template <typename U>
    void bind(U& object, R (U::*function)(T...))
    {
        // This function will be caleed by `Delegate::operator()`. It's responsible for calling the member function,
        // which is available through `Delegate::function` pointer on the object which is available through
        // `Delegate::object` pointer.
        dispatch = [](Delegate* self, T&&... args) -> R {

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

    /// Invokes the callee with the given arguments.
    R operator()(T&&... args)
    {
        if (!dispatch) {
            // TODO: report an error.
        }

        return dispatch(this, std::forward<T>(args)...);
    }

    /// Unbinds this `Delegate` from the callee.
    void unbind()
    {
        dispatch = nullptr;
    }

    /// Checks whether this `Delegate` is bound to anything.
    explicit operator bool() const
    {
        return nullptr != dispatch;
    }

  private:
    /// If this delegate is bound to a member function, holds a pointer to the object which member function this
    /// delegate is bound to, otherwise holds `nullptr`.
    void* object = nullptr;

    /// If this delegate is bound to a member function, holds a pointer to the member function. If this delegate is
    /// bound to a free-standing function, holds a pointer to that funciton, otherwise holds `nullptr`.
    void* function = nullptr;

    /// Holds a pointer to a member function that knows how to dispatch call to the callee. Holds `nullptr` if nothing
    /// has been bound to this delegate.
    R (*dispatch)(Delegate* self, T&&...) = nullptr;
};

} // namespace v1
} // namespace nest

