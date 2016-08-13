#include "sound.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: listen file" << std::endl;
        // std::cout << "Try `listen --help for more information`" << std::endl;
        exit(1);
    }
    // if (argv[0] == "help") {
    //     std::cout << "Usage: listen file" << std::endl;
    // }

    Sound* sound = new Sound(argv[1]);

    return 0;
}
