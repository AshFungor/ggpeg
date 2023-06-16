#include <rang.hpp>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <format>

// Local headers.
#include <clp-parser/clp-parser.hpp>
#include <image/image.hpp>
#include <processing/processing.hpp>

bool global_dry_run {true};

void img_processing(img::Image& main_image,
                    clpp::CommandType tp_command_type,
                    std::vector<std::string>& tp_param,
                    img::ImageType& file_format)
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
        global_dry_run = false;
        break;
    }

    case clpp::CommandType::rotate:
    {
        double degrees{std::stod(tp_param[0])};
        proc::rotate(main_image, degrees);
        global_dry_run = false;
        break;
    }

    case clpp::CommandType::resize:
    {
        double k{std::stod(tp_param[0])};
        proc::resize(main_image, k);
        global_dry_run = false;
        break;
    }

    case clpp::CommandType::negative:
    {
        proc::negative(main_image);
        global_dry_run = false;
        break;
    }
    case clpp::CommandType::insert:
    {
        double x_ins{std::stod(tp_param[0])};
        double y_ins{std::stod(tp_param[1])};
        std::string new_img{tp_param[2]};
        auto type = img::get_type(new_img);
        img::Image* ins_image;
        if (type == img::ImageType::PNG) {
            static img::PNGImage tp_new_image;
            ins_image = &tp_new_image;
        } else if (type == img::ImageType::PPM) {
            static img::PPMImage tp_new_image;
            ins_image = &tp_new_image;
        } else {
            throw std::runtime_error("File does not exist or has unsupported type");
        }
        ins_image->read(new_img);
        proc::insert(main_image, *ins_image, x_ins, y_ins);
        global_dry_run = false;
        break;
    }
    case clpp::CommandType::convert_to:
    {
        std::string new_format{tp_param[0]};
        if (new_format == "ppm")
        {
            main_image = img::convert(main_image, img::ImageType::PPM);
            file_format = img::ImageType::PPM;
        }
        else if (new_format == "png")
        {
            main_image = img::convert(main_image, img::ImageType::PNG);
            file_format = img::ImageType::PNG;
        } else {
            throw std::runtime_error(std::format("Unsupported file format: {}", new_format));
        }
        global_dry_run = false;
        break;
    }
    case clpp::CommandType::reflect_x:
    {
        proc::reflect_x(main_image);
        global_dry_run = false;
        break;
    }

    case clpp::CommandType::reflect_y:
    {
        proc::reflect_y(main_image);
        global_dry_run = false;
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
    if (argc == 2) {
        clpp::cerr_disabled = false;
        if (std::string{argv[1]} == "--help" || std::string{argv[1]} == "-h") {
            clpp::help();
        }
        else if (std::string{argv[1]} == "--version" || std::string{argv[1]} == "-v") {
            clpp::version();
        }
        else {
            std::cerr << "Unknown single command, use --help to see usage" << std::endl;
        }
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
        auto file_format = img::get_type(clpp::global_Path);
        img::Image* main_image;

        if (file_format == img::ImageType::PPM)
        {
            static img::PPMImage tp_image;
            tp_image.read(clpp::global_Path);
            main_image = &tp_image;
        }
        else if (file_format == img::ImageType::PNG)
        {
            static img::PNGImage tp_image;
            tp_image.read(clpp::global_Path);
            main_image = &tp_image;
        }
        else
        {
            throw std::runtime_error("File does not exist or is unsupported");
        }

        while(!queue_of_command.empty())
        {
            clpp::Command tp_command = queue_of_command.front();
            clpp::CommandType tp_command_type = tp_command.get_command();
            std::vector<std::string> tp_param = tp_command.get_param();
            
           
            img_processing(*main_image, tp_command_type, tp_param, file_format);
            
            queue_of_command.pop();
        }
        if (global_dry_run)
        {
            return 0;
        }
        const std::string new_path{std::format("target.{}", (file_format ==
                                                             img::ImageType::PNG) ? "png" : "ppm")};
        main_image->write(new_path);   
    }
    catch(const std::exception& e)
    {
        std::cerr << rang::bg::red
                  << "Execution failed with the following error:"
                  << rang::bg::reset << std::endl;
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
