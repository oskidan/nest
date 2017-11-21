#include <cstdlib>
#include <iostream>

#include <nest/opengl.hpp>

/*
Build:
    g++ -std=c++17 -Wall -Werror -I. test/opengl.cpp -lmingw32 -lSDL2main -lSDL2 -lglew32 -lopengl32

Expected result:
    A 320 x 240 window filled with red color.
*/

extern "C" int SDL_main(int argc, char* argv[])
{

    if (0 != SDL_Init(SDL_INIT_EVERYTHING)) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    std::atexit(SDL_Quit);

    nest::OpenGL context = nest::OpenGL::Builder{}
                               .with_window("Hello, World!", 320, 240)
                               .with_color(8, 8, 8, 8)
                               .with_depth_and_stencil(24, 8)
                               .with_version(4, 1)
                               .with(nest::OpenGL::Profile::Core)
                               .with(nest::OpenGL::DoubleBuffering::On);

    if (!context) {
        std::cerr << "Failed to initialize OpenGL 4.1 context: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    for (auto running = true; running;) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }

        glClearColor(1.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        context.swap_buffers();
    }

    return EXIT_SUCCESS;
}
