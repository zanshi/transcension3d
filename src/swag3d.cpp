#include <Application.hpp>

int main(int argc, char *argv[]) {
    // Application initialization
    sw::Application app;
    if (app.init()) {
        app.run();
    }

    return 0;
}
