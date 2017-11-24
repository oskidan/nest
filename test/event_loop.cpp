#include <cstdlib>
#include <iostream>

#include <glm/glm.hpp>

#include <nest/event_loop.hpp>
#include <nest/renderer_context.hpp>

/*
Build:
    g++ -std=c++17 -Wall -Werror -I. test/event_loop.cpp -lmingw32 -lSDL2main -lSDL2 -lglew32 -lopengl32

Expected result:
    A 320 x 240 window filled with changing color.
*/

extern "C" int SDL_main(int argc, char* argv[])
{

    if (0 != SDL_Init(SDL_INIT_EVERYTHING)) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    std::atexit(SDL_Quit);

    auto context = nest::make_renderer_context("Event Loop Test", 320, 240);
    if (!context) {
        std::cerr << "Failed to initialize OpenGL 4.1 context: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    nest::EventLoop event_loop;

    event_loop.on_tick.bind([&context](float const time_step) {
        static float a = 0.f;
        static float t = 0.f;

        a += 10.f * time_step;
        t += 20.f * time_step;

        // clang-format off
        if (a > 180.f) a -= 360.f;
        if (t > 180.f) t -= 360.f;
        // clang-format on

        auto const r = 0.5f * (glm::cos(glm::radians(a)) + 1.f);
        auto const b = 0.5f * (glm::sin(glm::radians(t)) + 1.f);

        glClearColor(r, 0.f, b, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        context.swap_buffers();
    });

    event_loop.run();

    return EXIT_SUCCESS;
}
