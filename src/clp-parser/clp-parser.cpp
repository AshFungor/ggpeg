
#include "clp-parser.hpp"



#define CERR if (cerr_disabled) {} else std::cerr

////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Definitions for Parser ////////////////////////////
////////////////////////////////////////////////////////////////////////////////


clpp::Parser::Parser(const std::vector<std::string>& tokens, const bool display)
{	
	clpp::cerr_disabled = display;
	if (tokens[0] == "--help" || tokens[0] == "-h")
	{ 
		clpp::help();
		throw std::runtime_error("");
	}
	else if (tokens[0] == "--version" || tokens[0] == "-v")
	{
		clpp::version();
		throw std::runtime_error("");
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
	_line_of_command.clear();
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
        CERR << path << std::endl;
		CERR << rang::bg::red;
		CERR << "ERROR:";
		CERR << rang::bg::reset;
		CERR << rang::fg::red;
        CERR << " There is no such file in this directory" << std::endl;
		CERR << rang::fg::reset;
		throw std::runtime_error("");
	}

	// check format
    std::string format{path.end() - 3, path.end()};
	if (std::find(allowed_Format.begin(), allowed_Format.end(), format) == allowed_Format.end())
	{
		CERR << path << std::endl;
		CERR << rang::bg::red;
		CERR << "ERROR:";
		CERR << rang::bg::reset;
		CERR << rang::fg::red;
        CERR << " GGPEG does not support the format of the file that you provided."
                     "\nFor the program to work correctly, select the file with the extension:"
                  << std::endl;
		for (size_t i{0}; i != allowed_Format.size(); ++i)
		{
            CERR << rang::fg::cyan << '.' << allowed_Format[i] << ' ';
		}

		CERR<< rang::fg::reset <<std::endl;
		
		throw std::runtime_error("");
		
	}
}
void clpp::Parser::_exceptions_for_incorrect_format(const std::string &token)
{	
	CERR << "--" <<  token << std::endl;
	CERR << rang::bg::red;
	CERR << "ERROR:";
	CERR << rang::bg::reset;
	CERR << rang::fg::red;
	CERR << " Is incorrect command. \nTo view the full list of available commands and rules," 
                 "type -h or --help during the next program launch."
              << std::endl;
	CERR << rang::fg::reset;
	throw std::runtime_error("");
	
}
void clpp::Parser::_exceptions_for_short_command(const char token)
{
	CERR << rang::bg::red;
	CERR << "ERROR:";
	CERR << rang::bg::reset;
	if (token == 'c')
	{ 	
		CERR << rang::fg::red;
		CERR << " There is no short <c> command. \nPerhaps you meant --crop or --convert_to." 
                     "\nTo view the full list of available commands, type -h or --help during "
                     "the next program launch."
                  << std::endl;
		CERR << rang::fg::reset;
		throw std::runtime_error("");
		
	}
	else if (token == 'r')
	{ 
		CERR << rang::fg::red;
		CERR << " There is no short <r> command. \nPerhaps you meant --rotate or --resize." 
                     "\nTo view the full list of available commands, type -h or --help during "
                     "the next program launch."
                  << std::endl;
		CERR << rang::fg::reset;
		throw std::runtime_error("");
		
	}
	else if (token == 'i')
	{ 
		CERR << rang::fg::red;
		CERR << " There is no short <i> command. \nPerhaps you meant --insert." 
                     "\nTo view the full list of available commands, type -h or --help "
                     "during the next program launch."
                  << std::endl;
		CERR << rang::fg::reset;
		throw std::runtime_error("");
		
	}
	else
	{ 
		CERR << rang::fg::red;
		CERR << " There is no <" << token << "> command." 
                     "\nTo view the full list of available commands, "
                     "type -h or --help during the next program launch."
                  << std::endl;
		CERR << rang::fg::reset;
		throw std::runtime_error("");
		
	}
}
std::string clpp::Parser::_convert_to_long(const char sh)
{
	if (sh == 'n') { return "negative"; };
	if (sh == 'x') { return "reflect_x"; };
	if (sh == 'y') { return "reflect_y"; };
	if (sh == 'v') { return "version"; };
	if (sh == 'h') { return "help"; };
	return "error";
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
	CERR << std::endl;
	CERR << rang::bg::cyan;
	CERR << "  TOKENS:  ";
	CERR << rang::bg::reset;
	CERR << std::endl;
	CERR << rang::fg::cyan;
	for(size_t i{0}; i != _tokens.size(); ++i)
	{
		CERR << _tokens[i] << std::endl;
	}
	CERR << rang::fg::reset;
}
void clpp::Parser::show_line_of_command()
{
	CERR << std::endl;
	CERR << rang::bg::cyan;
	CERR << "  COMMANDS:  ";
	CERR << rang::bg::reset;
	CERR << std::endl;
	CERR << rang::fg::cyan;
	for(size_t i{0}; i != _line_of_command.size(); ++i)
	{
		CERR << _line_of_command[i] << std::endl;
	}
	CERR << rang::fg::reset;
}
void clpp::Parser::show_queue_of_command()
{	
	CERR << std::endl;
	CERR << rang::bg::cyan;
	CERR << "  Queue elements:  ";
	CERR << rang::bg::reset;
	CERR << std::endl;
	std::queue<Command> queue_of_command_for_show{_queue_of_command};
	while(!queue_of_command_for_show.empty())
	{
		Command tp_command = queue_of_command_for_show.front();
		CERR << rang::fg::cyan;
		CERR << "--------------------------" << std::endl;
		CERR << rang::fg::reset;
		tp_command.show_command();
		tp_command.show_param();
		CERR << rang::fg::cyan;
		CERR << "--------------------------" << std::endl << std::endl;
		CERR << rang::fg::reset;
		queue_of_command_for_show.pop();
	}
	CERR << rang::fg::reset;

}
void clpp::Parser::show_correct_long_command()
{	
	CERR << std::endl;
	CERR << rang::bg::cyan;
	CERR << "  Correct long command:  ";
	CERR << rang::bg::reset; 
	CERR << std::endl;
	CERR << rang::fg::cyan;
	for (size_t i{0}; i != _correct_long_commands.size(); ++i)
	{
		CERR << _correct_long_commands[i] << std::endl;
	}
	CERR << rang::fg::reset;
}
void clpp::Parser::show_correct_short_command()
{	
	CERR << std::endl;
	CERR << rang::bg::cyan;
	CERR << "  Correct short command:  ";
	CERR << rang::bg::reset; 
	CERR << std::endl;
	CERR << rang::fg::cyan;
	for (char c : _correct_short_command)
	{
		CERR << c << std::endl;
	}
	CERR << rang::fg::reset;
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
}

clpp::Command::~Command()
{
	_param.clear();
}

void clpp::Command::_parser_param_crop(const std::string& second_part)
{	
	if (second_part.size() == 0) { _error_param(); }
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
	
	if (_param.size() != 4) { _error_param(); }

	int left = std::stoi(_param[0]);
	int top = std::stoi(_param[1]);
	int right = std::stoi(_param[2]);
	int bottom = std::stoi(_param[3]);

	if ((left + right) >= 100 || (top + bottom) >= 100) { _error_param(); }
}

void clpp::Command::_parser_param_rotate(const std::string& second_part)
{
	if (second_part.size() == 0) { _error_param(); }
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
	if (second_part.size() == 0) { _error_param(); }
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
	if (second_part.size() == 0) { _error_param(); }
	std::string str_tp_x_y;
	std::string path_to_new;

	size_t i = 0;
	if (second_part.find("#") < second_part.size()) 
	{	
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
			else { _error_param(); }
			++i;
		}
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
	if (global_Path.size() > 0)
	{
		std::string main_img_format{global_Path.end() - 3, global_Path.end()};
		std::string ins_img_format{path_to_new.end() - 3, path_to_new.end()};
		if (main_img_format != ins_img_format) { _error_param(); }
	}
	_exceptions_for_new_image(path_to_new);
	_param.emplace_back(path_to_new);
}
void clpp::Command::_parser_param_convert_to(const std::string& second_part)
{
	if (second_part.size() == 0) { _error_param(); }
	std::string new_format;

	for (size_t i{0}; i != second_part.size(); ++i)
	{
		new_format += second_part[i];
	}
	if (std::find(allowed_Format.begin(), allowed_Format.end(), new_format) == allowed_Format.end())
	{
		CERR << new_format << std::endl;
		CERR << rang::bg::red;
		CERR << "ERROR:";
		CERR << rang::bg::reset;
		CERR << rang::fg::red;
		CERR << "GGPEG does not support the format of the file that you provided." 
                     "\nFor the program to work correctly, select the file with the extension:"
                  << std::endl;
		CERR << rang::fg::reset;
		for (size_t i{0}; i != allowed_Format.size(); ++i)
		{
			CERR << rang::fg::cyan; 
			CERR << '.' <<allowed_Format[i] << ' '; 
		}
		CERR << std::endl;
		CERR << rang::fg::reset;
		throw std::runtime_error("");
		
	}
	_param.emplace_back(new_format);
}

void clpp::Command::_exceptions_for_new_image(const std::string &path)
{
	// Check exestence
	std::ifstream check_existence(path);
	if (!check_existence.good())
	{	
		CERR << path << std::endl;
		CERR << rang::bg::red;
		CERR << "ERROR:";
		CERR << rang::bg::reset;
		CERR << rang::fg::red;
		CERR << "There is no such file in this directory." << std::endl;
		CERR << rang::fg::reset;
		throw std::runtime_error("");
		
	}

	// check format
	std::string format{path.end()-3, path.end()};
	if (std::find(allowed_Format.begin(), allowed_Format.end(), format) == allowed_Format.end())
	{
		CERR << path << std::endl;
		CERR << rang::bg::red;
		CERR << "ERROR:";
		CERR << rang::bg::reset;
		CERR << rang::fg::red;
		CERR << " GGPEG does not support the format of the file that you provided." 
                     "\nFor the program to work correctly, select the file with the extension:"
                  << std::endl;
		for (size_t i{0}; i != allowed_Format.size(); ++i)
		{
			CERR << rang::fg::cyan << '.' <<allowed_Format[i] << ' '; 
		}
		CERR << std::endl;
		CERR << rang::fg::reset;
		throw std::runtime_error("");
		
	}
}
void clpp::Command::_error_param()
{	
	CERR << rang::bg::red;
	CERR << "ERROR:";
	CERR << rang::bg::reset;
	CERR << rang::fg::red;
	CERR << " Error entering command parameters."
			     "\nTo view the full list of available commands and his parametrs," 
			     "type -h or --help during the next program launch." 
			  << std::endl;
	CERR << rang::fg::reset;

	throw std::runtime_error("");
		
}
void clpp::Command::show_param()
{	
	CERR << rang::bg::cyan;
	CERR << "  Param of this command:  ";
	CERR << rang::bg::reset;
	CERR << std::endl;
	CERR << rang::fg::cyan;
	for (size_t i{0}; i != _param.size(); ++i)
	{
		CERR << _param[i] << std::endl;
	}
	CERR << rang::fg::reset;
}
void clpp::Command::show_command()
{
	CERR << rang::bg::cyan;
	CERR << "  Command is:  ";
	CERR << rang::bg::reset;
	CERR << std::endl;
	CERR << rang::fg::cyan;
	CERR << static_cast<int>(_command) << std::endl;
	CERR << rang::fg::reset;
}

std::vector<std::string> clpp::Command::get_param()
{
	return _param;
}	
clpp::CommandType clpp::Command::get_command()
{
	return _command;
}


/////////////////////////////////////////////////////////////////////////////////
/////////////////// Definitions for help and version functions //////////////////
/////////////////////////////////////////////////////////////////////////////////

void clpp::help()
{
	if (!clpp::cerr_disabled){
		std::cerr << std::endl;

		std::cerr << rang::fg::yellow 
					<< "---------------------------------------------------------" << std::endl
					<< "|" << " GGPEG is a command line utility for image processing. " << "|" << std::endl
					<< "---------------------------------------------------------" << std::endl
					<< "|" << " Utility can only be run with pre-passed commands.     " << "|" <<std::endl
					<< "---------------------------------------------------------" << std::endl
					<< rang::fg::reset << std::endl;
		
		std::cerr << rang::fg::green
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   " << rang::bg::green << "name" << rang::bg::reset << "        |   " << rang::bg::green << "parametrs" << rang::bg::reset << "       |   " << rang::bg::green << "description" << rang::bg::reset << "                            |   " << rang::bg::green << "short command" << rang::bg::reset << "   |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   crop        |   left_margin     |   crop left                              |   unavailable     |" << std::endl
					<< "|               |   top_margin      |   crop top                               |                   |" << std::endl
					<< "|               |   right_margin    |   crop right                             |                   |" << std::endl
					<< "|               |   bottom_margin   |   crop bottom                            |                   |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   FORMAT:     |  --crop=left_margin/top_margin/right_margin/bottom_margin                        |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   rotate      |   degrees         |   angle of rotation                      |   unavailable     |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   FORMAT:     |  --rotate=degrees                                                                |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   resize      |   k               |   multiplicity of resolution changes.    |   unavailable     |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   FORMAT:     |  --resize=0.3                                                                    |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   negative    |   -               |   filter negative                        |   n               |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   FORMAT:     |  --negative or -n                                                                |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   insert      |   x               |   insert x coordinate                    |   unavailable     |" << std::endl
					<< "|               |   y               |   insert y coordinate                    |                   |" << std::endl
					<< "|               |   path_to_new     |   path to the inserted image             |                   |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   FORMAT:     |  --insert=x/y#path_to_new                                                        |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   convert_to  |   new_format      |   one of the valid options: ppm or png   |   unavailable     |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   FORMAT:     |  --convert_to=new_format                                                            |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   reflect_x   |   -               |   reflection X                           |   x               |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   FORMAT:     |  --reflect_x or -x                                                               |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   reflect_y   |   -               |   reflection Y                           |   y               |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   FORMAT:     |  --reflect_y or -y                                                               |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   version     |   -               |   show version                           |   v               |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   FORMAT:     |  --version or -v                                                                 |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   help        |   -               |   show help message                      |   h               |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "|   FORMAT:     |  --help or -h                                                                    |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl;
		std::cerr << std::endl;
		std::cerr << rang::fg::green 
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "| " << rang::bg::green << "Rules" << rang::bg::reset <<   "                                                                                            |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl
					<< "| # All long commands start with --                                                                |" << std::endl
					<< "| # All short commands start with -                                                                |" << std::endl
					<< "| # Complex commands are written strictly separately, while simple ones can be combined,           |" << std::endl
					<< "| # for example: -hxv                                                                              |" << std::endl
					<< "----------------------------------------------------------------------------------------------------" << std::endl;
	}
}

void clpp::version()
{
	if (!clpp::cerr_disabled){
		std::cerr << std::endl;
		std::cerr << rang::fg::green << "------------------" << std::endl;
		std::cerr << "| " <<  "version: 1.0.0 |" << std::endl; 
		std::cerr << "------------------" << rang::fg::reset << std::endl;
	}
}