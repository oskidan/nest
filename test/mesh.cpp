#include <cstdlib>
#include <iostream>

#include <nest/event_loop.hpp>
#include <nest/renderer_context.hpp>

struct Vertex final {
    glm::vec2 position;

    Vertex(float const x, float const y) : position(x, y)
    {
    }
};

/*
c++ -std=c++17 -g -O0 -Wall -Werror -I. test/mesh.cpp -lmingw32 -lglew32 -lopengl32 -lSDL2main -lSDL2

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

    auto context = nest::make_renderer_context("Mesh Test", 320, 240);
    context.make_current();

    // TODO:
    //  - [ ] `Mesh::has_indices()`      to dsicover whether a `Mesh` contains any indices.
    //  - [ ] `Mesh::get_index_count()`  to discover the number of inidices in a `Mesh`.
    //  - [ ] `Mesh::get_vertex_count()` to discover the number of vertices in a `Mesh`.
    //
    //  Low priority:
    //  - [ ] Macros for vertex attribute locations, which can be used in both C++ and GLSL.

    nest::Mesh mesh =
        nest::Mesh::Builder{}.with_vertices({Vertex(-0.5f, -0.5f), Vertex(0.0f, 0.5f), Vertex(0.5f, -0.5f)});

    nest::ShaderProgram program = nest::ShaderProgram::Builder{}
                                      .with_vertex_shader(R"(#version 410
            layout(location = 0) in vec2 position;

            void main() {
                gl_Position = vec4(position, 0.0, 1.0);
            }
        )")
                                      .with_fragment_shader(R"(
            void main() {
                gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
            }
        )");

    nest::EventLoop event_loop;
    event_loop.on_tick.bind([&context, &program, &mesh](float time_step) {
        glClearColor(1.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.enable();
        mesh.enable();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        context.swap_buffers();
    });
    event_loop.run();

    return EXIT_SUCCESS;
}
