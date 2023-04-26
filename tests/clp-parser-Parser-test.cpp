#include <catch2/catch_test_macros.hpp>
#include <clp-parser/clp-parser.hpp>

#define gg_t_token std::vector<std::string>


TEST_CASE("The parser returns correct tokens", "[parser]")
{
	gg_t_token correct_tokens = {"test_png.png", "--rotate=90", "-hxy", "--insert=23/23#test_ppm.ppm", "--negative"};
	gg_t_token empty = {""};
	gg_t_token h = {"--help"};

	SECTION("All good", "[parser]")
	{
		clpp::Parser test_parser_1{correct_tokens};
		REQUIRE(test_parser_1.get_tokens() == correct_tokens);
	}
	SECTION("help")
	{
		gg_t_token h = {"--help"};

		REQUIRE_THROWS_AS(clpp::Parser(h), std::runtime_error);
	}
		
}


TEST_CASE("The parser returns correct line of commands", "[parse]")
{
	gg_t_token correct_tokens = {"test_png.png", "--crop=45/45/6/2", "--rotate=32", "--resize=0.2", 
									"--insert=23/23#test_ppm.ppm", "--convert_to=ppm", "-nxyvh"};

	gg_t_token correct_commands= {"crop=45/45/6/2", "rotate=32", "resize=0.2", "insert=23/23#test_ppm.ppm", 
									"convert_to=ppm", "negative", "reflect_x", "reflect_y", "version", "help"};
	gg_t_token empty = {""};

	clpp::Parser test_parser{correct_tokens};

	REQUIRE(test_parser.get_line_of_command() == correct_commands);
}


TEST_CASE("The parser returns correct queue of commands", "[parse]")
{
	gg_t_token correct_tokens = {"test_png.png", "--rotate=90", "-y", "--insert=23/32#test_ppm.ppm", "--crop=45/67/52/90"};
	gg_t_token correct_commands= {"rotate=90", "reflect_y", "insert=23/32#test_ppm.ppm", "crop=45/67/52/90"};

	std::queue<clpp::Command> test_queue;

	clpp::Parser test_parser{correct_tokens};

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