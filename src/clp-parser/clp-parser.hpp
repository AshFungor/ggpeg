#pragma once

#include <iostream>
#include <vector>
#include <string>

namespace clpp {
    // Code for CLI parser.

    //std::string globalPathToImage{""};

    // Parser class
    // Accepts vector and returns a queue of commands
    class Parser
    {
        public:
            std::vector<std::string> parse(std::vector<std::string> tokens);
    };

    // Enum with command
    enum CommandType
    {
        crop = 0, 
        rotate = 0, 
        resize = 0,
        negative = 0, // n
        insert = 0,
        convert_to = 0,
        reflect_x = 0, // x
        reflect_y = 0, // y
        version = 0, // v
        help = 0 // h
    };

    // Command class
    // Store type and arguments for command
    class Command
    {
        public:
            Command(std::string inputToken);
        
        private:
            std::string type; // Type of command
            std::vector<int> arg{NULL}; // Arguments for command. In default null;
        public: 

    };

}
