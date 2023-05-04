#include <rang.hpp>
#include <iostream>
#include <vector>


// Local headers.
#include <clp-parser/clp-parser.hpp>
#include <image/image.hpp>
#include <processing/processing.hpp>



int main(int argc, char** argv)
{   
    if(argc == 1)
	{
		std::cerr << rang::bg::yellow 
                    << "  GGPEG can only be run with pre-passed commands.  " 
                    << rang::bg::reset << std::endl;
		std::cerr << rang::fg::yellow 
                    << "To view the full list of available commands and rules,"
                    << "type -h or --help during the next program launch." 
                    << rang::fg::reset << std::endl;
        return 0;
	}
    // Vector with the values of the 
    // commands given when the program starts
    std::vector<std::string> input_Tokens;

    // Add tokens to vector
    for (size_t i = 1; i < argc; ++i)
    {
        input_Tokens.emplace_back(argv[i]);
    }
    // std::cerr<<inputTokens[0];
    try
    {   
        bool non_display = false;
        clpp::Parser parser {input_Tokens, non_display};
        
        std::queue<clpp::Command> queue_of_command = parser.get_queue_of_command();

        while(!queue_of_command.empty())
        {
            clpp::Command tp_command = queue_of_command.front();
            clpp::CommandType tp_command_type = tp_command.get_command();
            switch(tp_command_type)
            {
                case clpp::CommandType::crop:
                    std::cerr << "crop" << std::endl;
                    break;
                case clpp::CommandType::rotate:
                    std::cerr << "rotate" << std::endl;
                    break;
                case clpp::CommandType::resize:
                    std::cerr << "resize" << std::endl;
                    break;
                case clpp::CommandType::negative:
                    std::cerr << "negative" << std::endl;
                    break;
                case clpp::CommandType::insert:
                    std::cerr << "insert" << std::endl;
                    break;
                case clpp::CommandType::convert_to:
                    std::cerr << "convert_to" << std::endl;
                    break;
                case clpp::CommandType::reflect_x:
                    std::cerr << "reflect_x" << std::endl;
                    break;
                case clpp::CommandType::reflect_y:
                    std::cerr << "reflect_y" << std::endl;
                    break;
                case clpp::CommandType::version:
                    std::cerr << "version" << std::endl;
                    break;
                case clpp::CommandType::help:
                    std::cerr << "help" << std::endl;
                    break;
            }
            queue_of_command.pop();
        } 

        parser.show_queue_of_command();
    }
    catch(const std::exception& e)
    {

        return 0;
    }
    


    return 0;
}
