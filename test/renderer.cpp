#include <cstdlib>
#include <iostream>

#include <nest/renderer.hpp>

struct FakeContext final {

    bool valid = true;

    FakeContext() noexcept
    {
        std::cout << "FakeContext::FakeContext()\n";
    }

    ~FakeContext() noexcept
    {
        if (valid) {
            std::cout << "FakeContext::~FakeContext()\n";
        }
    }

    FakeContext(FakeContext&& that) : valid(that.valid)
    {
        that.valid = false;
    }

    void make_current()
    {
        if (valid) {
            std::cout << "make_current(FakeContext &)\n";
        }
    }
};

/*
Build:
    g++ -std=c++17 -Wall -Werror -I. test/renderer.cpp

Expected output:

    FakeContext::FakeContext()
    make_current(FakeContext &)
    Hello, World!
    Good bye, World!
    FakeContext::~FakeContext()
*/

int main(int const argc, char const* const argv[])
{
    FakeContext ctx;
    nest::AsyncRenderer renderer(std::move(ctx));
    nest::AsyncRenderer::CommandQueue::Builder builder;
    renderer.submit(builder.enqueue([] { std::cout << "Hello, "; }).enqueue([] { std::cout << "World!\n"; }));
    renderer.submit(builder.enqueue([] { std::cout << "Good bye, "; }).enqueue([] { std::cout << "World!\n"; }));
    // Gives the renderer a chance to run.
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return EXIT_SUCCESS;
}
