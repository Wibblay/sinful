#include <filesystem>
#include <iostream>

#include "Compiler.hpp"

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 3)
    {
        std::cerr << "Usage: SinfulApp.exe <input.sin> [output.asm]" << std::endl;
        return EXIT_FAILURE;
    }

    fs::path input(argv[1]);
    if (input.extension() != ".sin")
    {
        std::cerr << "Input file must have a .sin extension" << std::endl;
        return EXIT_FAILURE;
    }

    fs::path output = (argc == 3) ? fs::path(argv[2]) : input.parent_path() / input.stem().concat(".asm");

    Sinful::Compiler compiler(input.string(), output.string());
    return compiler.Compile();
}
