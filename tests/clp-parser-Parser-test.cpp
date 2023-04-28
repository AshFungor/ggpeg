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
		std::vector<std::string> incorrect_short_token_1 = {"test_png.png", "-q"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_1, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_2 = {"test_png.png", "-p"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_2, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_3 = {"test_png.png", "-g"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_3, non_display), std::runtime_error);
	}

	SECTION("Incorrect long commands", "[parser]")
	{
		std::vector<std::string> incorrect_short_token_1 = {"test_png.png", "--rtate=54"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_1, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_2 = {"test_png.png", "--qwerty"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_2, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_3 = {"test_png.png", "--x"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_3, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_4 = {"test_png.png", "--crop=54"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_4, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_5 = {"test_png.png", "--insert=54/34#12.ppm"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_5, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_6 = {"test_png.png", "--insert=54/34/test_png.png"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_6, non_display), std::runtime_error);

		std::vector<std::string> incorrect_short_token_7 = {"test_png.png", "--insert=54/34#error.ppm"};
        CHECK_THROWS_AS(clpp::Parser(incorrect_short_token_7, non_display), std::runtime_error);
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

	CHECK(test_parser.get_line_of_command() == correct_commands);
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
		CHECK(test_queue.size() == 4);
	}

	SECTION("Correct commands", "[parser]") 
	{
		CHECK(test_queue.front().get_command() == clpp::CommandType::rotate);
		gg_t_token test_param_1 = {"90"};
		CHECK(test_queue.front().get_param() == test_param_1);
		test_queue.pop();

		CHECK(test_queue.front().get_command() == clpp::CommandType::reflect_y);
		gg_t_token test_param_2;
		CHECK(test_queue.front().get_param() == test_param_2);
		test_queue.pop();

		CHECK(test_queue.front().get_command() == clpp::CommandType::insert);
		gg_t_token test_param_3 = {"23", "32", "test_ppm.ppm"};
		CHECK(test_queue.front().get_param() == test_param_3);
		test_queue.pop();

		CHECK(test_queue.front().get_command() == clpp::CommandType::crop);
		gg_t_token test_param_4 = {"45", "67", "52", "90"};
		CHECK(test_queue.front().get_param() == test_param_4);
		test_queue.pop();
	}
}

TEST_CASE("Class Command return correct type and param of commands", "[command]")
{
	SECTION("crop")
	{
		std::string crop{ "crop=12/34/12/56" }; 
		gg_t_token param_crop{"12", "34", "12", "56"};
		clpp::Command test_Command{crop};
		CHECK(test_Command.get_command() == clpp::CommandType::crop);
		CHECK(test_Command.get_param() == param_crop);
	}
	SECTION("rotate")
	{
		std::string rotate{ "rotate=78" }; 
		gg_t_token param_rotate{"78"};
		clpp::Command test_Command{rotate};
		CHECK(test_Command.get_command() == clpp::CommandType::rotate);
		CHECK(test_Command.get_param() == param_rotate);
	}
	SECTION("resize")
	{
		std::string resize{ "resize=0.4" }; 
		gg_t_token param_resize{"0.400000"};
		clpp::Command test_Command{resize};
		CHECK(test_Command.get_command() == clpp::CommandType::resize);
		CHECK(test_Command.get_param() == param_resize);
	}
	SECTION("negative")
	{
		std::string negative{ "negative" }; 
		gg_t_token param_negative{};
		clpp::Command test_Command{negative};
		CHECK(test_Command.get_command() == clpp::CommandType::negative);
		CHECK(test_Command.get_param() == param_negative);
	}
	SECTION("insert")
	{
		std::string insert{ "insert=12/34#test_png.png" }; 
		gg_t_token param_insert{"12", "34", "test_png.png"};
		clpp::Command test_Command{insert};
		CHECK(test_Command.get_command() == clpp::CommandType::insert);
		CHECK(test_Command.get_param() == param_insert);
	}
	SECTION("convert_to")
	{
		std::string convert_to{ "convert_to=ppm" }; 
		gg_t_token param_convert_to{"ppm"};
		clpp::Command test_Command{convert_to};
		CHECK(test_Command.get_command() == clpp::CommandType::convert_to);
		CHECK(test_Command.get_param() == param_convert_to);
	}
	SECTION("reflect_x")
	{
		std::string reflect_x{ "reflect_x" }; 
		gg_t_token param_reflect_x{};
		clpp::Command test_Command{reflect_x};
		CHECK(test_Command.get_command() == clpp::CommandType::reflect_x);
		CHECK(test_Command.get_param() == param_reflect_x);
	}
	SECTION("reflect_y")
	{
		std::string reflect_y{ "reflect_y" }; 
		gg_t_token param_reflect_y{};
		clpp::Command test_Command{reflect_y};
		CHECK(test_Command.get_command() == clpp::CommandType::reflect_y);
		CHECK(test_Command.get_param() == param_reflect_y);
	}
	SECTION("version")
	{
		std::string version{ "version" }; 
		gg_t_token param_version{};
		clpp::Command test_Command{version};
		CHECK(test_Command.get_command() == clpp::CommandType::version);
		CHECK(test_Command.get_param() == param_version);
	}
	SECTION("help")
	{
		std::string help{ "help" }; 
		gg_t_token param_help{};
		clpp::Command test_Command{help};
		CHECK(test_Command.get_command() == clpp::CommandType::help);
		CHECK(test_Command.get_param() == param_help);
	}
}

TEST_CASE("Class Command return incorrect param of commands", "[command]")
{
	SECTION("crop")
	{
		std::string crop_1{ "crop=12/a/12/56" }; 
		CHECK_THROWS_AS(clpp::Command(crop_1), std::runtime_error);

		std::string crop_2{ "crop=12/23//12/56" }; 
		CHECK_THROWS_AS(clpp::Command(crop_2), std::runtime_error);

		std::string crop_3{ "crop=12/23/12/56/" }; 
		CHECK_THROWS_AS(clpp::Command(crop_3), std::runtime_error);
	}
	SECTION("rotate")
	{
		std::string rotate_1{ "rotate=a" }; 
		CHECK_THROWS_AS(clpp::Command(rotate_1), std::runtime_error);

		std::string rotate_2{ "rotate=" }; 
		CHECK_THROWS_AS(clpp::Command(rotate_2), std::runtime_error);
	}
	SECTION("resize")
	{
		std::string resize_1{ "resize=-1" }; 
		CHECK_THROWS_AS(clpp::Command(resize_1), std::runtime_error);

		std::string resize_2{ "resize=" }; 
		CHECK_THROWS_AS(clpp::Command(resize_2), std::runtime_error);
	}
	SECTION("insert")
	{
		std::string insert_1{ "insert=12/34?test_png.png" }; 
		CHECK_THROWS_AS(clpp::Command(insert_1), std::runtime_error);

		std::string insert_2{ "insert=12/#test_png.png" }; 
		CHECK_THROWS_AS(clpp::Command(insert_2), std::runtime_error);

		std::string insert_3{ "insert=12%23#test_png.png" }; 
		CHECK_THROWS_AS(clpp::Command(insert_3), std::runtime_error);

		std::string insert_4{ "insert=/23#test_png.png" }; 
		CHECK_THROWS_AS(clpp::Command(insert_4), std::runtime_error);

		std::string insert_5{ "insert=23/23#test_p.png" }; 
		CHECK_THROWS_AS(clpp::Command(insert_5), std::runtime_error);

		std::string insert_6{ "insert=23/23test_p.png" }; 
		CHECK_THROWS_AS(clpp::Command(insert_6), std::runtime_error);

		std::string insert_7{ "insert=t/23test_p.png" }; 
		CHECK_THROWS_AS(clpp::Command(insert_7), std::runtime_error);
	}
	SECTION("convert_to")
	{
		std::string convert_1{ "convert_to=pp" }; 
		CHECK_THROWS_AS(clpp::Command(convert_1), std::runtime_error);

		std::string convert_2{ "convert_to=ppm/png" }; 
		CHECK_THROWS_AS(clpp::Command(convert_2), std::runtime_error);
	}
}

