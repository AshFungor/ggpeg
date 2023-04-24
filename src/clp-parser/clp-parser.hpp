#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <string>
#include <algorithm>
#include <queue>


/** \brief
 *  Namespaceclpp
 */
namespace clpp {
    // Code for CLI parser.

    // Path to image
    inline std::string global_Path;

    // Allowed formats
    inline std::vector<std::string> allowed_Format = {"ppm", "tga", "png"};


    // Enum with command
    enum class CommandType
    {
        crop = 0, 
        rotate = 1, 
        resize = 2,
        negative = 3, // n
        insert = 4,
        convert_to = 5,
        reflect_x = 6, // x
        reflect_y = 7, // y
        version = 8, // v
        help = 9 // h
    };

    // Command class
    // Store type and arguments for command
    class Command
    {
        public:
            Command(std::string& command);

            void show_param();
            
            
        private:
            CommandType _command;
            std::vector<std::string> _param;

            void _parser_param_crop(std::string& second_part);
            void _parser_param_rotate(std::string& second_part);
            void _parser_param_resize(std::string& second_part);
            void _parser_param_insert(std::string& second_part);
            void _parser_param_convert_to(std::string& second_part);
            void _exceptions_for_new_image(const std::string &path);
            void _error_param();

    };

    inline void helppp(){
        std::cerr<<"help.."<<std::endl;
    }

    // Parser class
    // Accepts vector and returns a queue of commands
    class Parser
    {
        public:
            Parser(const std::vector<std::string> &tokens);
            ~Parser();
            
            std::vector<std::string> get_tokens();
            std::vector<std::string> get_line_of_command();

            void show_tokens();
            void show_line_of_command();


        private:
            std::vector<std::string> _tokens;
            std::vector<std::string> _line_of_command;
            std::queue<Command> _queue_of_command;

            // Parsing tokens in a suitable vector format
            void _parse_to_string(); 
            std::string _convert_to_long(const char sh);
            void _parse_to_queue();

            // Check commands
            void _check_combination_short_command(const std::string &combination_short_command);
            void _check_long_command(const std::string &long_command);

            // Handling exceptions
            void _exceptions_for_image(const std::string &path);
            void _exceptions_for_short_command(const char token);
            void _exceptions_for_incorrect_format(const std::string &token);

    };


}
