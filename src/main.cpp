#include "app/App.hpp"

int main()
{
    try
    {
        App app;
        app.execute();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}