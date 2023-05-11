#include <catch2/catch_all.hpp>
#include <clp-parser/clp-parser.hpp>


bool non_display = true;

TEST_CASE("The parser returns correct tokens", "[parser]")
{
	
	std::vector<std::string> correct_tokens = {"resources/test_png.png", "--rotate=90", "-hxy", "--insert=23/23#resources/test_ppm.ppm", "--negative"};

	SECTION("All good", "[parser]")
	{
		clpp::Parser test_parser_1{correct_tokens, non_display};
		CHECK(test_parser_1.get_tokens() == correct_tokens);
	}

    SECTION("Help command", "[parser]")
    {
		std::vector<std::string> help_token_short = {"-h"};
        CHECK_THROWS_AS(clpp::Parser(help_token_short, non_display), std::runtime_error);
		
		std::vector<std::string> help_token_long = {"--help"};
        CHECK_THROWS_AS(clpp::Parser(help_token_long, non_display), std::runtime_error);
    }


	SECTION("Incorrect short commands", "[parser]")
	{
		std::vector<std::string> incorrect_short_token_1 = {"resources/test_png.png", "-q"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_1, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_2 = {"resources/test_png.png", "-p"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_2, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_3 = {"resources/test_png.png", "-g"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_3, non_display), std::runtime_error);
	}

	SECTION("Incorrect long commands", "[parser]")
	{
		std::vector<std::string> incorrect_short_token_1 = {"resources/test_png.png", "--rtate=54"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_1, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_2 = {"resources/test_png.png", "--qwerty"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_2, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_3 = {"resources/test_png.png", "--x"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_3, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_4 = {"resources/test_png.png", "--crop=54"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_4, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_5 = {"resources/test_png.png", "--insert=54/34#12.ppm"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_5, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_6 = {"resources/test_png.png", "--insert=54/34/test_png.png"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_6, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_7 = {"resources/test_png.png", "--insert=54/34#error.ppm"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_7, non_display), std::runtime_error);
	}

}

TEST_CASE("The parser returns correct line of commands", "[parse]")
{
	std::vector<std::string> correct_tokens = {"resources/test_png.png", "--crop=45/45/6/2", "--rotate=32", "--resize=0.2", 
									"--insert=23/23#resources/test_ppm.ppm", "--convert_to=ppm", "-nxyvh"};

	std::vector<std::string> correct_commands= {"crop=45/45/6/2", "rotate=32", "resize=0.2", "insert=23/23#resources/test_ppm.ppm", 
									"convert_to=ppm", "negative", "reflect_x", "reflect_y", "version", "help"};
	std::vector<std::string> empty = {""};

	clpp::Parser test_parser{correct_tokens, non_display};

	CHECK(test_parser.get_line_of_command() == correct_commands);
}


TEST_CASE("The parser returns correct queue of commands", "[parse]")
{
	std::vector<std::string> correct_tokens = {"resources/test_png.png", "--rotate=90", "-y", "--insert=23/32#resources/test_ppm.ppm", "--crop=45/67/52/90"};
	std::vector<std::string> correct_commands= {"rotate=90", "reflect_y", "insert=23/32#resources/test_ppm.ppm", "crop=45/67/52/90"};

	std::queue<clpp::Command> test_queue;

	clpp::Parser test_parser{correct_tokens, non_display};

	test_queue = test_parser.get_queue_of_command();

	SECTION("Size of queue", "[parser]")
	{
		CHECK(test_queue.size() == 4);
	}

	SECTION("Correct commands", "[parser]") 
	{
		CHECK(test_queue.front().get_command() == clpp::CommandType::rotate);
		std::vector<std::string> test_param_1 = {"90"};
		CHECK(test_queue.front().get_param() == test_param_1);
		test_queue.pop();

		CHECK(test_queue.front().get_command() == clpp::CommandType::reflect_y);
		std::vector<std::string> test_param_2;
		CHECK(test_queue.front().get_param() == test_param_2);
		test_queue.pop();

		CHECK(test_queue.front().get_command() == clpp::CommandType::insert);
		std::vector<std::string> test_param_3 = {"23", "32", "resources/test_ppm.ppm"};
		CHECK(test_queue.front().get_param() == test_param_3);
		test_queue.pop();

		CHECK(test_queue.front().get_command() == clpp::CommandType::crop);
		std::vector<std::string> test_param_4 = {"45", "67", "52", "90"};
		CHECK(test_queue.front().get_param() == test_param_4);
		test_queue.pop();
	}
}

