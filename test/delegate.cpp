#include <iostream>

#include <nest/delegate.hpp>

char const* say(char const* message)
{
    std::cout << message;
    return "World!";
}

class Echo {
  public:
    char const* echo(char const* message)
    {
        return message;
    }
};

struct CaptureMe {
    bool valid = true;

    char const* x = "your name?";

    CaptureMe() noexcept = default;

    CaptureMe(CaptureMe const&)
    {
    }

    CaptureMe(CaptureMe&& that)
    {
        that.valid = false;
    }

    ~CaptureMe() noexcept
    {
        if (valid) {
            std::cout << "CaptureMe::~CaptureMe" << std::endl;
        }
    }
};

/*
Build:
    c++ -std=c++17 -Wall -Werror -I. test/delegate.cpp
*/

int main(int const argc, char const* const argv[])
{
    nest::Delegate<char const*(char const*)> delegate;
    delegate.bind(say);
    std::cout << "Delegate says: " << delegate("Hello, ") << std::endl;

    Echo echo;
    delegate.bind(echo, Echo::echo);
    std::cout << "Delegate says: " << delegate("Bye") << std::endl;

    delegate.bind([](char const* what) -> char const* {
        std::cout << what;
        return "meet you!";
    });
    std::cout << "Delegate says: " << delegate("Glad to ") << std::endl;

    CaptureMe c;
    delegate.bind([c](char const* what) -> char const* {
        std::cout << what;
        return c.x;
    });
    std::cout << "Delegate says: " << delegate("Don't you know ") << std::endl;

    delegate.bind([c = std::move(c)](char const* what) -> char const* {
        std::cout << what;
        return c.x;
    });
    std::cout << "Delegate says: " << delegate("What is ") << std::endl;

    CaptureMe d;
    delegate.bind(
        [](void* user_data, char const* what) -> char const* {
            std::cout << what;
            return static_cast<CaptureMe*>(user_data)->x;
        },
        &d);
    std::cout << "Delegate says: " << delegate("What is ") << std::endl;

    return 0;
}
