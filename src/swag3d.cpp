#include <Application.hpp>

#undef main

int main(int argc, char *argv[]) {
    // Application initialization

    std::vector<std::string> args(argv, argv+argc);

    std::string input;

    //std::cout << args.size() << std::endl;

    if(args.size() == 2) {
        input = args[1];
        std::cout << "Input: " << input << std::endl;
    } else {
        // TODO: handle scene loading more gracefully
        std::cout << "COLLADA file located in the /res folder:" << std::endl;
        std::cin >> input;
    }

    sw::Application app;
    if (app.init()) {
        app.run(input);
    }

    return 0;
}
