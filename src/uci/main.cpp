#include <iostream>

int main(int argc, char* argv[])
{
    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i];
        if (i < argc - 1) {
            std::cout << " ";
        }
    }

    std::cout << std::endl;

    return 0;
}
