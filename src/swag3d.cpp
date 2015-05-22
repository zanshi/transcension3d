#include <Application.hpp>

#undef main

int main(int argc, char *argv[]) {
    // Application initialization

    std::vector<std::string> args(argv, argv+argc);


    sw::Application app;
    if (app.init()) {
        app.run(args);
    }

    return 0;
}
