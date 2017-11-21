#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace nest {
inline namespace v1 {

/// A class for executing renderer commands on seprate thread.
class AsyncRenderer final {
  public:
    /// A type for representing any renderer command.
    using Command = std::function<void()>;

    /// A class for storing commands in a queue. Instances of this class are immutable, use the nested `Builder` class
    /// to construct them.
    class CommandQueue final {
      public:
        class Builder;

        /// Constructs an empty queue of commands.
        CommandQueue() noexcept = default;

        /// Executes commands in the queue.
        void execute()
        {
            // Current strategy is not to break execution when an exception gets thrown by a command.
            // TODO: figure out if this strategy is wrong or if it's desirable to support other strategies as well.

            std::for_each(begin(commands), end(commands), [](auto& fn) {
                try {
                    fn();
                }
                catch (...) {
                    // TODO: report an error.
                }
            });
        }

      private:
        /// Holds the enqueued commands.
        std::vector<Command> commands;
    };

    /// Constructs a renderer instance with the given context.
    template <typename Context>
    AsyncRenderer(Context&& ctx)
    {
        thread = std::thread([ this, ctx = std::move(ctx) ]() mutable {
            ctx.make_current();
            loop();
        });
    }

    ~AsyncRenderer() noexcept
    {
        running.store(false);

        awake.notify_one();

        if (thread.joinable()) {
            thread.join();
        }
    }

    /// Submits the given command queue for execution.
    void submit(CommandQueue&& queue)
    {
        std::unique_lock lock(queues_mutex);
        queues.emplace_back(std::move(queue));
    }

  private:
    /// 'Infinite' loop which executes renderer commands.
    void loop()
    {
        using std::chrono::high_resolution_clock;

        // Holds the desired duration of a single frame.
        auto const desired_duration = high_resolution_clock::duration(std::chrono::nanoseconds(1'000'000'000 / 60));

        // Holds a zero-length duration.
        auto const zero_duration = high_resolution_clock::duration::zero();

        auto then = std::chrono::high_resolution_clock::now();

        while (running.load()) {
            decltype(queues) queues;
            {
                std::unique_lock lock(queues_mutex);
                std::swap(queues, this->queues);
            }

            std::for_each(begin(queues), end(queues), [](auto& q) { q.execute(); });

            auto const now = std::chrono::high_resolution_clock::now();

            // Specifies how much time passed since last tick.
            auto const frame_duration = now - then;

            if (auto const mod = frame_duration % desired_duration; zero_duration == mod) {
                std::unique_lock lock(awake_mutex);
                awake.wait_for(lock, mod);
            }
        }
    }

    /// Holds a handle to a thread on which the renderer commands will be
    /// executed.
    std::thread thread;

    /// Holds a handle to a mutex which protects `awake`.
    std::mutex awake_mutex;

    /// Is used to wake up the thread.
    std::condition_variable awake;

    /// Holds a handle to a mutex which protects `queues`.
    std::mutex queues_mutex;

    /// Holds the queues, submitted for execution.
    std::vector<CommandQueue> queues;

    /// Holds a boolean which specifies whether the thread shall be running.
    std::atomic_bool running{true};
};

/// A class for constructing instances of `CommandQueue` class.
class AsyncRenderer::CommandQueue::Builder final {
  public:
    /// Appends a new command into the queue. The command is constructed with the given arguments.
    template <typename... T>
    Builder& enqueue(T&&... args)
    {
        queue.commands.emplace_back(std::forward<T>(args)...);
        return *this;
    }

    /// \returns The constructed `CommandQueue` instance.
    operator CommandQueue()
    {
        return std::move(queue);
    }

  private:
    /// Holds the queue which is being built.
    CommandQueue queue;
};

} // inline namespace v1
} // namespace nest
