#include <catch2/catch_test_macros.hpp>
#include <clp-parser/clp-parser.hpp>

#define gg_t_token std::vector<std::string>
bool non_display = true;

TEST_CASE("The parser returns correct tokens", "[parser]")
{
	
	gg_t_token correct_tokens = {"test_png.png", "--rotate=90", "-hxy", "--insert=23/23#test_ppm.ppm", "--negative"};

	SECTION("All good", "[parser]")
	{
		clpp::Parser test_parser_1{correct_tokens, non_display};
		REQUIRE(test_parser_1.get_tokens() == correct_tokens);
	}

    SECTION("Help command", "[parser]")
    {
		
		std::vector<std::string> help_token_short = {"-h"};
        REQUIRE_THROWS_AS(clpp::Parser(help_token_short, non_display), std::runtime_error);
		
		std::vector<std::string> help_token_long = {"--help"};
        REQUIRE_THROWS_AS(clpp::Parser(help_token_long, non_display), std::runtime_error);
    }


	SECTION("Incorrect short commands", "[parser]")
	{
		std::vector<std::string> incorrect_short_token_1 = {"test_png.png", "-q"};
        REQUIRE_THROWS_AS(clpp::Parser(incorrect_short_token_1, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_2 = {"test_png.png", "-p"};
        REQUIRE_THROWS_AS(clpp::Parser(incorrect_short_token_2, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_3 = {"test_png.png", "-g"};
        REQUIRE_THROWS_AS(clpp::Parser(incorrect_short_token_3, non_display), std::runtime_error);
	}

	SECTION("Incorrect long commands", "[parser]")
	{
		std::vector<std::string> incorrect_short_token_1 = {"test_png.png", "--rtate=54"};
        REQUIRE_THROWS_AS(clpp::Parser(incorrect_short_token_1, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_2 = {"test_png.png", "--qwerty"};
        REQUIRE_THROWS_AS(clpp::Parser(incorrect_short_token_2, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_3 = {"test_png.png", "--x"};
        REQUIRE_THROWS_AS(clpp::Parser(incorrect_short_token_3, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_4 = {"test_png.png", "--crop=54"};
        REQUIRE_THROWS_AS(clpp::Parser(incorrect_short_token_4, non_display), std::runtime_error);

		//std::vector<std::string> incorrect_short_token_5 = {"test_png.png", "--insert=54/34#12.ppm"};
        //REQUIRE_THROWS_AS(clpp::Parser(incorrect_short_token_5), std::runtime_error);

		//std::vector<std::string> incorrect_short_token_6 = {"test_png.png", "--insert=54/34/test_png.png"};
        //REQUIRE_THROWS_AS(clpp::Parser(incorrect_short_token_6), std::runtime_error);

		std::vector<std::string> incorrect_short_token_7 = {"test_png.png", "--insert=54/34#error.ppm"};
        REQUIRE_THROWS_AS(clpp::Parser(incorrect_short_token_7, non_display), std::runtime_error);
	}

}


TEST_CASE("The parser returns correct line of commands", "[parse]")
{
	gg_t_token correct_tokens = {"test_png.png", "--crop=45/45/6/2", "--rotate=32", "--resize=0.2", 
									"--insert=23/23#test_ppm.ppm", "--convert_to=ppm", "-nxyvh"};

	gg_t_token correct_commands= {"crop=45/45/6/2", "rotate=32", "resize=0.2", "insert=23/23#test_ppm.ppm", 
									"convert_to=ppm", "negative", "reflect_x", "reflect_y", "version", "help"};
	gg_t_token empty = {""};

	clpp::Parser test_parser{correct_tokens, non_display};

	REQUIRE(test_parser.get_line_of_command() == correct_commands);
}


TEST_CASE("The parser returns correct queue of commands", "[parse]")
{
	gg_t_token correct_tokens = {"test_png.png", "--rotate=90", "-y", "--insert=23/32#test_ppm.ppm", "--crop=45/67/52/90"};
	gg_t_token correct_commands= {"rotate=90", "reflect_y", "insert=23/32#test_ppm.ppm", "crop=45/67/52/90"};

	std::queue<clpp::Command> test_queue;

	clpp::Parser test_parser{correct_tokens, non_display};

	test_queue = test_parser.get_queue_of_command();

	SECTION("Size of queue", "[parser]")
	{
		REQUIRE(test_queue.size() == 4);
	}

	SECTION("Correct commands", "[parser]") 
	{
		REQUIRE(test_queue.front().get_command() == clpp::CommandType::rotate);
		gg_t_token test_param_1 = {"90"};
		REQUIRE(test_queue.front().get_param() == test_param_1);
		test_queue.pop();

		REQUIRE(test_queue.front().get_command() == clpp::CommandType::reflect_y);
		gg_t_token test_param_2;
		REQUIRE(test_queue.front().get_param() == test_param_2);
		test_queue.pop();

		REQUIRE(test_queue.front().get_command() == clpp::CommandType::insert);
		gg_t_token test_param_3 = {"23", "32", "test_ppm.ppm"};
		REQUIRE(test_queue.front().get_param() == test_param_3);
		test_queue.pop();

		REQUIRE(test_queue.front().get_command() == clpp::CommandType::crop);
		gg_t_token test_param_4 = {"45", "67", "52", "90"};
		REQUIRE(test_queue.front().get_param() == test_param_4);
		test_queue.pop();
	}

}
