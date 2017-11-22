#pragma once

#include <atomic>
#include <chrono>
#include <thread>

#include <SDL2/SDL.h>

#include <nest/delegate.hpp>

namespace nest {
inline namespace v1 {

/// A class for handling OS events.
class EventLoop {
  public:
    /// If this delegate is bound, and the user is closing the window, then the delegate will be called. Call
    /// `EventLoop::quit()` from the delegate to terminate the event loop. If this delegate is not bound, and the user
    /// is closing the window, then the event loop will terminate by default.
    Delegate<void()> on_quit;

    /// Gets called when the application is idle. Takes `time_step` in seconds since the last `on_tick` call. This
    /// delegate is not called unless something is bound to it.
    Delegate<void(float const time_step)> on_tick;

    /// Runs event loop. The loop will terminate when:
    ///     - `EventLoop::on_quit` is not bound and the user closes the window;
    ///     - `EventLoop::quit()`  is called.
    void run()
    {
        using std::chrono::high_resolution_clock;

        running.store(true);

        // Holds the desired duration of a single frame.
        auto desired_duration = high_resolution_clock::duration(std::chrono::nanoseconds(1'000'000'000 / 60));

        // Holds a zero-length duration.
        auto const zero_duration = high_resolution_clock::duration::zero();

        auto then = high_resolution_clock::now();

        while (running.load()) {
            process_events();

            auto const now = high_resolution_clock::now();

            // Specifies how much time passed since last tick.
            auto const frame_duration = now - then;

            if (on_tick) {
                using Seconds = std::chrono::duration<float>;
                auto const time_step = std::chrono::duration_cast<Seconds>(frame_duration).count();
                on_tick(time_step);
            }

            if (auto const mod = frame_duration % desired_duration; zero_duration == mod) {
                std::this_thread::sleep_for(mod);
            }

            then = now;
        }
    }

    /// Forces the event loop to terminate. This is normally what you'd call from `EventLoop::on_quit`.
    void quit()
    {
        running.store(false);
    }

  private:
    /// Loops through OS event queue and calls event handles on those events. This loop lasts until the OS event queue
    /// is exhausted.
    void process_events()
    {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                if (on_quit) {
                    on_quit();
                }
                else {
                    running.store(false);
                }
                break;

            default:
                // TODO: implement other event handlers.
                break;
            }
        }
    }

    /// Specifies whether or not the event loop is running.
    std::atomic_bool running = false;
};

} // namespace v1
} // namespace nest

