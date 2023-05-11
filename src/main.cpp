#include <rang.hpp>
#include <iostream>
#include <vector>


// Local headers.
#include <clp-parser/clp-parser.hpp>
#include <image/image.hpp>
#include <processing/processing.hpp>


std::string format( std::string_view path_to_image)
{
    std::string format{path_to_image.end() - 3, path_to_image.end()};

    if (format == "ppm") { return "ppm"; }
    else                 { return "png"; }
}




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
            auto tp_param = tp_command.get_param();

            std::string new_path{""}; 
            new_path = "new_" + clpp::global_Path;
            
            if (format(clpp::global_Path) == "ppm")
            {
                img::PPMImage tp_image;
                tp_image.read(clpp::global_Path);
                img::Image& main_image = static_cast<img::Image&>(tp_image);

                switch(tp_command_type)
                {
                    case clpp::CommandType::crop: 
                    {
                        double left_margin{std::stod(tp_param[0])};
                        double top_margin{std::stod(tp_param[1])};
                        double right_margin{std::stod(tp_param[2])};
                        double bottom_margin{std::stod(tp_param[3])};
                        std::cerr<<"crop"<<std::endl;
                        proc::crop(main_image, left_margin, top_margin, right_margin, bottom_margin);
                        main_image.write(new_path);
                        break;
                    }

                    case clpp::CommandType::rotate:
                    {
                        std::cerr<<"rotate"<<std::endl;
                        std::cerr<<tp_param[0];
                        double degrees{std::stod(tp_param[0])};
                        proc::rotate(main_image, degrees);
                        main_image.write(new_path);
                        break;
                    }

                    case clpp::CommandType::resize:
                    {
                        std::cerr<<"resize"<<std::endl;
                        
                        double k{std::stod(tp_param[0])};
                        
                        proc::resize(main_image, 3);
                        main_image.write(new_path);
                        break;
                    }

                    case clpp::CommandType::negative:
                    {
                        std::cerr<<"negative"<<std::endl;
                        proc::negative(main_image);
                        main_image.write(new_path);
                        break;
                    }
                    case clpp::CommandType::insert:  
                    {
                        std::cerr<<"insert"<<std::endl;
                        
                        
                        double x_ins{std::stod(tp_param[0])};
                        double y_ins{std::stod(tp_param[1])};
                        std::string new_imag{tp_param[2]};

                        if (format(new_imag) == "ppm")
                        {
                            img::PPMImage tp_new_image;
                            tp_new_image.read(clpp::global_Path);
                            img::Image& ins_image = static_cast<img::Image&>(tp_new_image);

                            proc::insert(main_image, ins_image, x_ins, y_ins);
                            main_image.write(new_path);
                        }
                        else if (format(clpp::global_Path) == "png")
                        {
                            img::PNGImage tp_new_image;
                            tp_new_image.read(clpp::global_Path);
                            img::Image& ins_image = static_cast<img::Image&>(tp_new_image);

                            proc::insert(main_image, ins_image, x_ins, y_ins);
                            main_image.write(new_path);
                        }
                        break;
                    }
                    case clpp::CommandType::convert_to:
                    {
                        std::cerr << "convert_to" << std::endl;
                        break;
                    }
                    case clpp::CommandType::reflect_x: 
                    {
                        std::cerr<<"reflect_x"<<std::endl;
                        proc::reflex_x(main_image);
                        main_image.write(new_path);
                        break;
                    }
                
                    case clpp::CommandType::reflect_y: 
                    {
                        std::cerr<<"reflect_y"<<std::endl;
                        proc::reflex_y(main_image);
                        main_image.write(new_path);
                        break;
                    }
                    
                    case clpp::CommandType::version: 
                        std::cerr << "version" << std::endl;
                        break;
                    
                    case clpp::CommandType::help: 
                        std::cerr << "help" << std::endl;
                        break;
                }
            }
            else if (format(clpp::global_Path) == "png")
            {
                img::PNGImage tp_image;
                tp_image.read(clpp::global_Path);
                img::Image& main_image = static_cast<img::Image&>(tp_image);
                switch(tp_command_type)
                {
                    case clpp::CommandType::crop: 
                    {
                        double left_margin{std::stod(tp_param[0])};
                        double top_margin{std::stod(tp_param[1])};
                        double right_margin{std::stod(tp_param[2])};
                        double bottom_margin{std::stod(tp_param[3])};
                        
                        proc::crop(main_image, left_margin, top_margin, right_margin, bottom_margin);
                        main_image.write(new_path);
                        break;
                    }

                    case clpp::CommandType::rotate:
                    {
                        double degrees{std::stod(tp_param[0])};
                        proc::rotate(main_image, degrees);
                        main_image.write(new_path);
                        break;
                    }

                    case clpp::CommandType::resize:
                    {
                        double k{std::stod(tp_param[0])};
                        proc::resize(main_image, k);
                        main_image.write(new_path);
                        break;
                    }

                    case clpp::CommandType::negative:
                    {
                        proc::negative(main_image);
                        main_image.write(new_path);
                        break;
                    }
                    case clpp::CommandType::insert:  
                    {
                        std::string new_imag{tp_param[0]};
                        double x_ins{std::stod(tp_param[1])};
                        double y_ins{std::stod(tp_param[2])};

                        if (format(new_imag) == "ppm")
                        {
                            img::PPMImage tp_new_image;
                            tp_new_image.read(clpp::global_Path);
                            img::Image& ins_image = static_cast<img::Image&>(tp_new_image);

                            proc::insert(main_image, ins_image, x_ins, y_ins);
                            main_image.write(new_path);
                        }
                        else if (format(clpp::global_Path) == "png")
                        {
                            img::PNGImage tp_new_image;
                            tp_new_image.read(clpp::global_Path);
                            img::Image& ins_image = static_cast<img::Image&>(tp_new_image);

                            proc::insert(main_image, ins_image, x_ins, y_ins);
                            main_image.write(new_path);
                        }
                        break;
                    }
                    case clpp::CommandType::convert_to:
                    {
                        std::cerr << "convert_to" << std::endl;
                        break;
                    }
                    case clpp::CommandType::reflect_x: 
                    {
                        proc::reflex_x(main_image);
                        main_image.write(new_path);
                        break;
                    }
                
                    case clpp::CommandType::reflect_y: 
                    {
                        proc::reflex_y(main_image);
                        main_image.write(new_path);
                        break;
                    }
                    
                    case clpp::CommandType::version: 
                        std::cerr << "version" << std::endl;
                        break;
                    
                    case clpp::CommandType::help: 
                        std::cerr << "help" << std::endl;
                        break;
                }
            }
            queue_of_command.pop();
        }    
    }
    catch(const std::exception& e)
    {
        return 0;
    }
    return 0;
}
