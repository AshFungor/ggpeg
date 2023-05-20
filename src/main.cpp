#include <rang.hpp>
#include <iostream>
#include <vector>


// Local headers.
#include <clp-parser/clp-parser.hpp>
#include <image/image.hpp>
#include <processing/processing.hpp>


std::string format( std::string& path_to_image)
{
    std::string format{path_to_image.end() - 3, path_to_image.end()};

    if (format == "ppm") { return "ppm"; }
    else                 { return "png"; }
}

void img_processing(img::Image& main_image, clpp::CommandType tp_command_type, std::vector<std::string>& tp_param, std::string& new_path, std::string& file_format)
{
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
                
                proc::resize(main_image, 3);
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
                double x_ins{std::stod(tp_param[0])};
                double y_ins{std::stod(tp_param[1])};
                std::string new_imag{tp_param[2]};
                
                if (file_format == "ppm")
                {
                    img::PPMImage tp_new_image;
                    tp_new_image.read(new_imag);
                    img::Image& ins_image = static_cast<img::Image&>(tp_new_image);

                    proc::insert(main_image, ins_image, x_ins, y_ins);
                    main_image.write(new_path);
                }
                else if (file_format == "png")
                {
                    img::PNGImage tp_new_image;
                    tp_new_image.read(new_imag);
                    img::Image& ins_image = static_cast<img::Image&>(tp_new_image);

                    proc::insert(main_image, ins_image, x_ins, y_ins);
                    main_image.write(new_path);
                }
                break;
            }
            case clpp::CommandType::convert_to:
            {
                break;
            }
            case clpp::CommandType::reflect_x: 
            {
                proc::reflect_x(main_image);
                main_image.write(new_path); 
                break;
            }
        
            case clpp::CommandType::reflect_y: 
            {
                proc::reflect_y(main_image);
                main_image.write(new_path);
                break;
            }
            
            case clpp::CommandType::version: 
                clpp::version();
                break;
            
            case clpp::CommandType::help: 
                clpp::help();
                break;
        }
}

std::string path_forming(std::string& gl_path, std::string& file_format)
{
    std::string new_path{""}; 
    std::string name{""};
    size_t i{gl_path.size() - 5};

    while (gl_path[i] != '/')
    {
        name = gl_path[i] + name;
        i--;
    }
    for (int x{1}; x != 1000000; ++x)
    {
        std::ifstream check_existence(name + "(" + std::to_string(x) + ")." + file_format);
	    if (!check_existence.good())
        {
            name = name + "(" + std::to_string(x) + ")." + file_format;
            break;
        }
	
    }
    new_path = name;
    
    return new_path;
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
    
    try
    {   
        bool non_display = false;
        clpp::Parser parser {input_Tokens, non_display};
        std::queue<clpp::Command> queue_of_command = parser.get_queue_of_command();
        std::string file_format = format(clpp::global_Path);

        if (file_format == "ppm")
        {
            img::PPMImage tp_image;
            tp_image.read(clpp::global_Path);
            img::Image& main_image = static_cast<img::Image&>(tp_image);
            
            std::string new_path{""};
            new_path = path_forming(clpp::global_Path, file_format);
            while(!queue_of_command.empty())
            {
                clpp::Command tp_command = queue_of_command.front();
                clpp::CommandType tp_command_type = tp_command.get_command();
                std::vector<std::string> tp_param = tp_command.get_param();

                img_processing(main_image, tp_command_type, tp_param, new_path, file_format);

                queue_of_command.pop(); 
            }
        }
        else if (file_format == "png")
        {
            img::PNGImage tp_image;
            tp_image.read(clpp::global_Path);
            img::Image& main_image = static_cast<img::Image&>(tp_image);

            std::string new_path{""};
            new_path = path_forming(clpp::global_Path, file_format);
            
            while(!queue_of_command.empty())
            {
                clpp::Command tp_command = queue_of_command.front();
                clpp::CommandType tp_command_type = tp_command.get_command();
                std::vector<std::string> tp_param = tp_command.get_param();

                
                img_processing(main_image, tp_command_type, tp_param, new_path, file_format);

                queue_of_command.pop(); 
            }
            
        }
          
    }
    catch(const std::exception& e)
    {
        return 0;
    }
    return 0;
}
