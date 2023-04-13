#include "clp-parser.hpp"

#include <vector>

// Definitions for CLI parser.


std::vector<std::string> clpp::Parser::parse(std::vector<std::string> tokens)
{	

	std::vector<std::string> goodTokens;
	

	for (size_t i = 1; i < tokens.size(); ++i)
	{	
		if (tokens[i][1] == '-') // Processing a complicated command
		{	
			goodTokens.emplace_back(tokens[i]);
		}
		else // Processing a simple command
		{
			for (size_t j = 1; j < tokens[i].size(); ++j) // Decoding the token into complicated commands
			{	
				if (tokens[i][j] == 'n') { goodTokens.emplace_back("--negative"); }
				else if (tokens[i][j] == 'x') { goodTokens.emplace_back("--reflect_x"); }
				else if (tokens[i][j] == 'y') { goodTokens.emplace_back("--reflect_y"); }
				else if (tokens[i][j] == 'v') { goodTokens.emplace_back("--version"); }
				else if (tokens[i][j] == 'h') { goodTokens.emplace_back("--help"); }
			}
		}
	}

	return goodTokens; 
}
