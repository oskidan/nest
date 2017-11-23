#include <cstdlib>
#include <iostream>

#include <nest/event_loop.hpp>
#include <nest/renderer_context.hpp>

/*
c++ -std=c++17 -g -O0 -Wall -Werror -I. test/shader_program.cpp -lmingw32 -lglew32 -lopengl32 -lSDL2main -lSDL2

Expected result:
    A yellow triangle on a red background drawn in the center of the screen.
*/

int SDL_main(int argc, char* argv[])
{
    if (0 != SDL_Init(SDL_INIT_EVERYTHING)) {
        std::cerr << "Cannot initialize SDL: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    std::atexit(SDL_Quit);

    auto context = nest::make_renderer_context("Shader Program Test", 320, 240);
    context.make_current();

    // NOTE: GL_3DL_array_objects extension is required by this shader program. I have no idea how wide spread that
    // extension is.
    nest::ShaderProgram program = nest::ShaderProgram::Builder{}
                                      .with_vertex_shader(R"(#version 410
            #extension GL_3DL_array_objects : require

            const vec2[3] position = vec2[]( 
                vec2(-0.5, -0.5),
                vec2( 0.0,  0.5),
                vec2( 0.5, -0.5)
            );

            void main() {
                gl_Position = vec4(position[gl_VertexID], 0.0, 1.0);
            }
        )").with_fragment_shader(R"(
            void main() {
                gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
            }
        )");

    nest::EventLoop event_loop;
    event_loop.on_tick.bind([&context, &program](float time_step) {
        glClearColor(1.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.enable();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        context.swap_buffers();
    });
    event_loop.run();

    return EXIT_SUCCESS;
}

