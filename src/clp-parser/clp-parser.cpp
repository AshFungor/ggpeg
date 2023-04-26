#include <rang.hpp>
#include "clp-parser.hpp"



////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Definitions for Parser ////////////////////////////
////////////////////////////////////////////////////////////////////////////////


clpp::Parser::Parser(const std::vector<std::string>& tokens)
{	
	
	if (tokens[0] == "--help" || tokens[0] == "-h")
	{ 
		clpp::helppp(); 
		exit(EXIT_FAILURE);
	}
	else { global_Path = tokens[0]; }

	_exceptions_for_image(global_Path);

	for (size_t i{1}; i != tokens.size(); ++i)
	{
		if (tokens[i][0] != '-') { _exceptions_for_incorrect_format(tokens[i]); }
		if (tokens[i][1] == '-') { _check_long_command(tokens[i]);              }
		else                     { _check_combination_short_command(tokens[i]); }
	}

	_tokens = tokens;
	_parse_to_string();
}
clpp::Parser::~Parser()
{
	_tokens.clear();
}
void clpp::Parser::_check_combination_short_command(const std::string &combination_short_command)
{
	for (size_t j{1}; j != combination_short_command.size(); ++j)
	{
		if (_correct_short_command.find(combination_short_command[j]) == std::string::npos)
		{ 
			_exceptions_for_short_command(combination_short_command[j]); 
		}
	}
}

void clpp::Parser::_check_long_command(const std::string &long_command)
{	
	
	size_t separator = long_command.find('=');
    std::string command = long_command.substr(2, separator - 2);

    if (std::find(_correct_long_commands.begin(), _correct_long_commands.end(), command)
            == _correct_long_commands.end())
	{
		_exceptions_for_incorrect_format(command);
	}
}

void clpp::Parser::_exceptions_for_image(const std::string &path)
{		
    // Check existence
	std::ifstream check_existence(path);
	if (!check_existence.good())
	{	
        std::cerr << path << std::endl;
		std::cerr << rang::bg::red;
		std::cerr << "ERROR:";
		std::cerr << rang::bg::reset;
		std::cerr << rang::fg::red;
        std::cerr << " There is no such file in this directory" << std::endl;
		std::cerr << rang::fg::reset;
		exit(EXIT_FAILURE);
	}

	// check format
    std::string format{path.end() - 3, path.end()};
	if (std::find(allowed_Format.begin(), allowed_Format.end(), format) == allowed_Format.end())
	{
		std::cerr << path << std::endl;
		std::cerr << rang::bg::red;
		std::cerr << "ERROR:";
		std::cerr << rang::bg::reset;
		std::cerr << rang::fg::red;
        std::cerr << " GGPEG does not support the format of the file that you provided."
                     "\nFor the program to work correctly, select the file with the extension:"
                  << std::endl;
		for (size_t i{0}; i != allowed_Format.size(); ++i)
		{
            std::cerr << rang::fg::cyan << '.' << allowed_Format[i] << ' ';
		}

		std::cerr<< rang::fg::reset <<std::endl;
		
		exit(EXIT_FAILURE);
	}
}

void clpp::Parser::_exceptions_for_incorrect_format(const std::string &token)
{	
	std::cerr << "--" <<  token << std::endl;
	std::cerr << rang::bg::red;
	std::cerr << "ERROR:";
	std::cerr << rang::bg::reset;
	std::cerr << rang::fg::red;
	std::cerr << " Is incorrect command. \nTo view the full list of available commands and rules," 
                 "type -h or --help during the next program launch."
              << std::endl;
	std::cerr << rang::fg::reset;
	exit(EXIT_FAILURE);
}

void clpp::Parser::_exceptions_for_short_command(const char token)
{
	std::cerr << rang::bg::red;
	std::cerr << "ERROR:";
	std::cerr << rang::bg::reset;
	if (token == 'c')
	{ 	
		std::cerr << rang::fg::red;
		std::cerr << " There is no short <c> command. \nPerhaps you meant --crop or --convert_to." 
                     "\nTo view the full list of available commands, type -h or --help during "
                     "the next program launch."
                  << std::endl;
		std::cerr << rang::fg::reset;
		exit(EXIT_FAILURE);
	}
	else if (token == 'r')
	{ 
		std::cerr << rang::fg::red;
		std::cerr << " There is no short <r> command. \nPerhaps you meant --rotate or --resize." 
                     "\nTo view the full list of available commands, type -h or --help during "
                     "the next program launch."
                  << std::endl;
		std::cerr << rang::fg::reset;
		exit(EXIT_FAILURE);
	}
	else if (token == 'i')
	{ 
		std::cerr << rang::fg::red;
		std::cerr << " There is no short <i> command. \nPerhaps you meant --insert." 
                     "\nTo view the full list of available commands, type -h or --help "
                     "during the next program launch."
                  << std::endl;
		std::cerr << rang::fg::reset;
		exit(EXIT_FAILURE);
	}
	else
	{ 
		std::cerr << rang::fg::red;
		std::cerr << " There is no <" << token << "> command." 
                     "\nTo view the full list of available commands, "
                     "type -h or --help during the next program launch."
                  << std::endl;
		std::cerr << rang::fg::reset;
		exit(EXIT_FAILURE);
	}
}

std::string clpp::Parser::_convert_to_long(const char sh)
{
	if (sh == 'n') { return "negative"; };
	if (sh == 'x') { return "reflect_x"; };
	if (sh == 'y') { return "reflect_y"; };
	if (sh == 'v') { return "version"; };
	if (sh == 'h') { return "help"; };
// Clang говорит warning: Adding 'const char' to a string does not append to the string
// что ты хотел сделать этой строкой?
	std::string error {"error" + sh};
	return error;
}

void clpp::Parser::_parse_to_string()
{
	for (size_t i{1}; i != _tokens.size(); ++i){
		if (_tokens[i][1] == '-')
		{ 
			std::string com{_tokens[i].begin()+2, _tokens[i].end()};
			_line_of_command.emplace_back(com); 
		}
		else                      
		{ 
			for (size_t j{1}; j != _tokens[i].size(); ++j)
			{
				_line_of_command.emplace_back(_convert_to_long(_tokens[i][j])); 
			}
		}
			
	}
	//show_line_of_command();
	_parse_to_queue();
}

void clpp::Parser::_parse_to_queue()
{
	for (size_t i{0}; i != _line_of_command.size(); ++i)
	{
		Command temp_command{_line_of_command[i]};
		_queue_of_command.push(temp_command);
	}
}

void clpp::Parser::show_tokens()
{
	std::cerr << std::endl;
	std::cerr << rang::bg::cyan;
	std::cerr << "  TOKENS:  ";
	std::cerr << rang::bg::reset;
	std::cerr << std::endl;
	std::cerr << rang::fg::cyan;
	for(size_t i{0}; i != _tokens.size(); ++i)
	{
		std::cerr << _tokens[i] << std::endl;
	}
	std::cerr << rang::fg::reset;
}
void clpp::Parser::show_line_of_command()
{
	std::cerr << std::endl;
	std::cerr << rang::bg::cyan;
	std::cerr << "  COMMANDS:  ";
	std::cerr << rang::bg::reset;
	std::cerr << std::endl;
	std::cerr << rang::fg::cyan;
	for(size_t i{0}; i != _line_of_command.size(); ++i)
	{
		std::cerr << _line_of_command[i] << std::endl;
	}
	std::cerr << rang::fg::reset;
}
void clpp::Parser::show_queue_of_command()
{	
	std::cerr << std::endl;
	std::cerr << rang::bg::cyan;
	std::cerr << "  Queue elements:  ";
	std::cerr << rang::bg::reset;
	std::cerr << std::endl;
	std::queue<Command> queue_of_command_for_show{_queue_of_command};
	while(!queue_of_command_for_show.empty())
	{
		Command tp_command = queue_of_command_for_show.front();
		std::cerr << rang::fg::cyan;
		std::cerr << "--------------------------" << std::endl;
		std::cerr << rang::fg::reset;
		tp_command.show_command();
		tp_command.show_param();
		std::cerr << rang::fg::cyan;
		std::cerr << "--------------------------" << std::endl << std::endl;
		std::cerr << rang::fg::reset;
		queue_of_command_for_show.pop();
	}
	std::cerr << rang::fg::reset;

}
void clpp::Parser::show_correct_long_command()
{	
	std::cerr << std::endl;
	std::cerr << rang::bg::cyan;
	std::cerr << "  Correct long command:  ";
	std::cerr << rang::bg::reset; 
	std::cerr << std::endl;
	std::cerr << rang::fg::cyan;
	for (size_t i{0}; i != _correct_long_commands.size(); ++i)
	{
		std::cerr << _correct_long_commands[i] << std::endl;
	}
	std::cerr << rang::fg::reset;
}
void clpp::Parser::show_correct_short_command()
{	
	std::cerr << std::endl;
	std::cerr << rang::bg::cyan;
	std::cerr << "  Correct short command:  ";
	std::cerr << rang::bg::reset; 
	std::cerr << std::endl;
	std::cerr << rang::fg::cyan;
	for (char c : _correct_short_command)
	{
		std::cerr << c << std::endl;
	}
	std::cerr << rang::fg::reset;
}

std::vector<std::string> clpp::Parser::get_tokens()
{
	return _tokens;
}
std::vector<std::string> clpp::Parser::get_line_of_command()
{
	return _line_of_command;
}
std::queue<clpp::Command> clpp::Parser::get_queue_of_command()
{
	return _queue_of_command;
}



/////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Definitions for Command ////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

clpp::Command::Command(std::string& command)
{	
	size_t separator = command.find('=');
	std::string first_part = command.substr(0, separator);
	if (first_part == "crop") 
	{ 
		_command = clpp::CommandType::crop; 
		std::string second_part = command.substr(separator+1, command.size());
		_parser_param_crop(second_part);
		
	}
	else if (first_part == "rotate") 
	{ 
		_command = clpp::CommandType::rotate; 
		std::string second_part = command.substr(separator+1, command.size());
		_parser_param_rotate(second_part);
	}
	else if (first_part == "resize") 
	{ 
		_command = clpp::CommandType::resize;
		std::string second_part = command.substr(separator+1, command.size());
		_parser_param_resize(second_part);
	}
	else if (first_part == "negative") { _command = clpp::CommandType::negative; }
	else if (first_part == "insert") 
	{ 
		_command = clpp::CommandType::insert; 
		std::string second_part = command.substr(separator+1, command.size());
		_parser_param_insert(second_part);
	}
	else if (first_part == "convert_to") 
	{ 
		_command = clpp::CommandType::convert_to; 
		std::string second_part = command.substr(separator+1, command.size());
		_parser_param_convert_to(second_part);
	}
	else if (first_part == "reflect_x") { _command = clpp::CommandType::reflect_x; }
	else if (first_part == "reflect_y") { _command = clpp::CommandType::reflect_y; }
	else if (first_part == "version")   { _command = clpp::CommandType::version;   }
	else if (first_part == "help")      { _command = clpp::CommandType::help;      }

	//show_param();

	//show_command();
}
void clpp::Command::_parser_param_crop(const std::string& second_part)
{	
	std::string str_tp_param;
	for (size_t i{0}; i != second_part.size(); ++i)
	{	
		if (isdigit(second_part[i])) { str_tp_param += second_part[i]; }
		else if (second_part[i] == '/' && i+1 != second_part.size() && second_part[i+1] != '/') 
		{ 
			if (str_tp_param.size() > 0)
			{
				int int_tp_param{std::stoi(str_tp_param)};
				if (int_tp_param > 0 && int_tp_param < 100) { _param.emplace_back(str_tp_param); }
				else                                        { _error_param();                    }
				str_tp_param = "";
			}
			else{ _error_param(); }
		}
		else{ _error_param(); }
	}
	if (str_tp_param.size() > 0)
	{
		int int_tp_param{std::stoi(str_tp_param)};
		if (int_tp_param > 0 && int_tp_param < 100) { _param.emplace_back(str_tp_param); }
		else                                        { _error_param();                    }
	}
	else{ _error_param(); }
}

void clpp::Command::_parser_param_rotate(const std::string& second_part)
{
	std::string str_tp_param;
	for (size_t i{0}; i != second_part.size(); ++i)
	{	
        if (isdigit(second_part[i]) || (second_part[i] == '-' && i == 0))
        { str_tp_param += second_part[i]; }
		else { _error_param(); }
	}
	if (str_tp_param.size() > 0)
	{
		int int_tp_param{std::stoi(str_tp_param)};
		int_tp_param %= 360;
		if (int_tp_param < 0) { int_tp_param += 360; }
		_param.emplace_back(std::to_string(int_tp_param));
	}
	else{ _error_param(); }
}
void clpp::Command::_parser_param_resize(const std::string& second_part)
{
	std::string str_tp_param;
	for (size_t i{0}; i != second_part.size(); ++i)
	{
        if (isdigit(second_part[i]) ||
                (second_part[i] == '.' && i+1 != second_part.size() && second_part[i+1] != '.'))
        { str_tp_param += second_part[i]; }
		else{ _error_param(); }
	}
	if (str_tp_param.size() > 0)
	{
		double double_tp_param{std::stod(str_tp_param)};
		_param.emplace_back(std::to_string(double_tp_param));
	}
}
void clpp::Command::_parser_param_insert(const std::string& second_part)
{
	std::string str_tp_x_y;
	std::string path_to_new;

	size_t i = 0;
	while (second_part[i] != '#')
	{
		if (isdigit(second_part[i])) { str_tp_x_y += second_part[i]; }
		else if (second_part[i] == '/' && i+1 != second_part.size() && second_part[i+1] != '/') 
		{ 
			if (str_tp_x_y.size() > 0)
			{
				int int_tp_x_y{std::stoi(str_tp_x_y)};
				_param.emplace_back(str_tp_x_y);
				str_tp_x_y = "";
			}
			else { _error_param(); }
		}		
		++i;
	}
	if (str_tp_x_y.size() > 0)
	{
		int int_tp_x_y{std::stoi(str_tp_x_y)};
		_param.emplace_back(str_tp_x_y);
		str_tp_x_y = "";
	}
	else { _error_param(); }
	++i;
	for (size_t j{i}; j != second_part.size(); ++j)
	{
		path_to_new += second_part[j];
	}
	_exceptions_for_new_image(path_to_new);
	_param.emplace_back(path_to_new);
}
void clpp::Command::_parser_param_convert_to(const std::string& second_part)
{
	std::string new_format;

	for (size_t i{0}; i != second_part.size(); ++i)
	{
		new_format += second_part[i];
	}
	if (std::find(allowed_Format.begin(), allowed_Format.end(), new_format) == allowed_Format.end())
	{
		std::cerr << new_format << std::endl;
		std::cerr << rang::bg::red;
		std::cerr << "ERROR:";
		std::cerr << rang::bg::reset;
		std::cerr << rang::fg::red;
		std::cerr <<"GGPEG does not support the format of the file that you provided." 
                  << "\nFor the program to work correctly, select the file with the extension:"
                  << std::endl;
		std::cerr << rang::fg::reset;
		for (size_t i{0}; i != allowed_Format.size(); ++i)
		{
			std::cerr << rang::fg::cyan; 
			std::cerr << '.' <<allowed_Format[i] << ' '; 
		}
		std::cerr << std::endl;
		std::cerr << rang::fg::reset;
		exit(EXIT_FAILURE);
	}
}

void clpp::Command::_exceptions_for_new_image(const std::string &path)
{
	// Check exestence
	std::ifstream check_existence(path);
	if (!check_existence.good())
	{	
		std::cerr << path << std::endl;
		std::cerr << rang::bg::red;
		std::cerr << "ERROR:";
		std::cerr << rang::bg::reset;
		std::cerr << rang::fg::red;
		std::cerr << "There is no such file in this directory." << std::endl;
		std::cerr << rang::fg::reset;
		exit(EXIT_FAILURE);
	}

	// check format
	std::string format{path.end()-3, path.end()};
	if (std::find(allowed_Format.begin(), allowed_Format.end(), format) == allowed_Format.end())
	{
		std::cerr << path << std::endl;
		std::cerr << rang::bg::red;
		std::cerr << "ERROR:";
		std::cerr << rang::bg::reset;
		std::cerr << rang::fg::red;
		std::cerr << "GGPEG does not support the format of the file that you provided." 
                  << "\nFor the program to work correctly, select the file with the extension:"
                  << std::endl;
		for (size_t i{0}; i != allowed_Format.size(); ++i)
		{
			std::cerr << rang::fg::cyan << '.' <<allowed_Format[i] << ' '; 
		}
		std::cerr << std::endl;
		std::cerr << rang::fg::reset;
		exit(EXIT_FAILURE);
	}
}
void clpp::Command::_error_param()
{	
	std::cerr << rang::bg::red;
	std::cerr << "ERROR:";
	std::cerr << rang::bg::reset;
	std::cerr << rang::fg::red;
	std::cerr << " Error entering command parameters."
			  << "\nTo view the full list of available commands and his parametrs," 
			  << "type -h or --help during the next program launch." << std::endl;
	std::cerr << rang::fg::reset;
	exit(EXIT_FAILURE);	
}
void clpp::Command::show_param()
{	
	std::cerr << rang::bg::cyan;
	std::cerr << "  Param of this command:  ";
	std::cerr << rang::bg::reset;
	std::cerr << std::endl;
	std::cerr << rang::fg::cyan;
	for (size_t i{0}; i != _param.size(); ++i)
	{
		std::cerr << _param[i] << std::endl;
	}
	std::cerr << rang::fg::reset;
}
void clpp::Command::show_command()
{
	std::cerr << rang::bg::cyan;
	std::cerr << "  Command is:  ";
	std::cerr << rang::bg::reset;
	std::cerr << std::endl;
	std::cerr << rang::fg::cyan;
	std::cerr << static_cast<int>(_command) << std::endl;
	std::cerr << rang::fg::reset;
}

std::vector<std::string> clpp::Command::get_param()
{
	return _param;
}	
clpp::CommandType clpp::Command::get_command()
{
	return _command;
}
