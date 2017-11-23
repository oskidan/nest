#include <cstdlib>
#include <iostream>

#include <glm/glm.hpp>

#include <nest/vertex_traits.hpp>

struct Vertex {
    float position[3];
    glm::vec3 color;
    glm::vec2 texcoord;
    int blah;
};

struct Dummy {
};

struct Evil {
    int position()
    {
        return 0;
    }

    float color()
    {
        return 0.f;
    }

    bool texcoord()
    {
        return false;
    }
};

/*
c++ -std=c++17 -g -O0 -Wall -Werror -I. test/vertex_traits.cpp

Expected output:
    1 0 0
    1 0 0
    1 0 0
    5126 5126 5126
    3 3 2 1
*/

int main(int const argc, char const* const argv[])
{
    // clang-format off
    std::cout << nest::has_position<Vertex> << " " 
              << nest::has_position<Dummy>  << " "
              << nest::has_position<Evil>   << std::endl;
    // clang-format on

    // clang-format off
    std::cout << nest::has_color<Vertex> << " " 
              << nest::has_color<Dummy>  << " "
              << nest::has_color<Evil>   << std::endl;
    // clang-format on

    // clang-format off
    std::cout << nest::has_texcoord<Vertex> << " " 
              << nest::has_texcoord<Dummy>  << " "
              << nest::has_texcoord<Evil>   << std::endl;
    // clang-format on

    // clang-format off
    std::cout << nest::opengl_type<decltype(Vertex::position)> << " "
              << nest::opengl_type<decltype(Vertex::color   )> << " "
              << nest::opengl_type<decltype(Vertex::texcoord)> << std::endl;
    // clang-format on

    // clang-format off
    std::cout << nest::component_count<decltype(Vertex::position)> << " "
              << nest::component_count<decltype(Vertex::color   )> << " "
              << nest::component_count<decltype(Vertex::texcoord)> << " "
              << nest::component_count<decltype(Vertex::blah    )> << std::endl;
    // clang-format on

    return EXIT_SUCCESS;
}

