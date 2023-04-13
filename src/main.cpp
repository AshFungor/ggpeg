#include <iostream>

#include <vector>

// Local headers.
#include <clp-parser/clp-parser.hpp>
#include <image/image.hpp>
#include <processing/processing.hpp>

int main(int argc, char** argv)
{   
    // Vector with the values of the 
    // commands given when the program starts
    std::vector<std::string> inputTokens;

    // Add tokens to vector
    for (size_t i = 1; i < argc; ++i)
    {
        inputTokens.emplace_back(argv[i]);
    }

    // Path to image
    std::string image = inputTokens[0];

    clpp::Parser parser {};
    std::vector<std::string> goodTokens = parser.parse(inputTokens);

    for (size_t i = 0; i!=goodTokens.size(); ++i)
    {
        std::cout<<goodTokens[i]<<std::endl;
    }

    return 0;
}
