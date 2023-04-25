#include <iostream>
#include <vector>
#include <rang.hpp>

// Local headers.
#include <clp-parser/clp-parser.hpp>
#include <image/image.hpp>
#include <processing/processing.hpp>



int main(int argc, char** argv)
{   
    // Vector with the values of the 
    // commands given when the program starts
    std::vector<std::string> input_Tokens;

    // test (REMOVE PLS)
    std::cout << rang::fg::green << "I AM SHREK\n";

    // Add tokens to vector
    for (size_t i = 1; i < argc; ++i)
    {
        input_Tokens.emplace_back(argv[i]);
    }
    // std::cerr<<inputTokens[0];
    clpp::Parser parser {input_Tokens};

    return 0;
}
