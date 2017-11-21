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

int main(int const argc, char const* const argv[])
{
    nest::Delegate<char const*(char const*)> delegate;
    delegate.bind(say);
    std::cout << "Delegate says: " << delegate("Hello, ") << std::endl;

    Echo echo;
    delegate.bind(echo, Echo::echo);
    std::cout << "Delegate says: " << delegate("Bye") << std::endl;
    return 0;
}
