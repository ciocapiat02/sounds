#include <iostream>

int main (int argc, char *argv[]) {
    if(argc < 2){
        std::cerr << "Usage: sounds {filename}" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
    std::cout << filename << std::endl;
    return 0;
}
