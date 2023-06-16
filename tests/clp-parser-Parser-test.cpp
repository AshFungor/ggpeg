#include <catch2/catch_all.hpp>
#include <clp-parser/clp-parser.hpp>


bool non_display = true;

TEST_CASE("The parser returns correct tokens", "[parser]")
{
	
	std::vector<std::string> correct_tokens = {"resources/clouds.png", "--rotate=90", "-hxy", "--insert=23/23#resources/clouds.png", "--negative"};

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
		std::vector<std::string> incorrect_short_token_1 = {"resources/clouds.png", "-q"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_1, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_2 = {"resources/clouds.png", "-p"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_2, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_3 = {"resources/clouds.png", "-g"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_3, non_display), std::runtime_error);
	}

	SECTION("Incorrect long commands", "[parser]")
	{
		std::vector<std::string> incorrect_short_token_1 = {"resources/clouds.png", "--rtate=54"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_1, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_2 = {"resources/clouds.png", "--qwerty"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_2, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_3 = {"resources/clouds.png", "--x"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_3, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_4 = {"resources/clouds.png", "--crop=54"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_4, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_5 = {"resources/clouds.png", "--insert=54/34#stop.ppm"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_5, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_6 = {"resources/clouds.png", "--insert=54/34/clouds.png"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_6, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_7 = {"resources/clouds.png", "--insert=54/34#error.ppm"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_7, non_display), std::runtime_error);
	}

}

TEST_CASE("The parser returns correct line of commands", "[parse]")
{
	std::vector<std::string> correct_tokens = {"resources/clouds.png", "--crop=45/45/6/2", "--rotate=32", "--resize=0.2", 
									"--insert=23/23#resources/clouds.png", "--convert_to=ppm", "-nxyvh"};

	std::vector<std::string> correct_commands= {"crop=45/45/6/2", "rotate=32", "resize=0.2", "insert=23/23#resources/clouds.png", 
									"convert_to=ppm", "negative", "reflect_x", "reflect_y", "version", "help"};
	std::vector<std::string> empty = {""};

	clpp::Parser test_parser{correct_tokens, non_display};

	CHECK(test_parser.get_line_of_command() == correct_commands);
}


TEST_CASE("The parser returns correct queue of commands", "[parse]")
{
	std::vector<std::string> correct_tokens = {"resources/clouds.png", "--rotate=90", "-y", "--insert=23/32#resources/clouds.png", "--crop=10/10/10/10"};
	std::vector<std::string> correct_commands= {"rotate=90", "reflect_y", "insert=23/32#resources/clouds.png", "crop=10/10/10/10"};

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
		std::vector<std::string> test_param_3 = {"23", "32", "resources/clouds.png"};
		CHECK(test_queue.front().get_param() == test_param_3);
		test_queue.pop();

		CHECK(test_queue.front().get_command() == clpp::CommandType::crop);
		std::vector<std::string> test_param_4 = {"10", "10", "10", "10"};
		CHECK(test_queue.front().get_param() == test_param_4);
		test_queue.pop();
	}
}

