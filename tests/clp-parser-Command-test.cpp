#include <catch2/catch_test_macros.hpp>
#include <clp-parser/clp-parser.hpp>


TEST_CASE("Class Command return correct type and param of commands", "[command]")
{
	SECTION("crop")
	{
		std::string crop{ "crop=12/34/12/56" }; 
		std::vector<std::string> param_crop{"12", "34", "12", "56"};
		clpp::Command test_Command{crop};
		CHECK(test_Command.get_command() == clpp::CommandType::crop);
		CHECK(test_Command.get_param() == param_crop);
	}
	SECTION("rotate")
	{
		std::string rotate{ "rotate=78" }; 
		std::vector<std::string> param_rotate{"78"};
		clpp::Command test_Command{rotate};
		CHECK(test_Command.get_command() == clpp::CommandType::rotate);
		CHECK(test_Command.get_param() == param_rotate);
	}
	SECTION("resize")
	{
		std::string resize{ "resize=0.4" }; 
		std::vector<std::string> param_resize{"0.400000"};
		clpp::Command test_Command{resize};
		CHECK(test_Command.get_command() == clpp::CommandType::resize);
		CHECK(test_Command.get_param() == param_resize);
	}
	SECTION("negative")
	{
		std::string negative{ "negative" }; 
		std::vector<std::string> param_negative{};
		clpp::Command test_Command{negative};
		CHECK(test_Command.get_command() == clpp::CommandType::negative);
		CHECK(test_Command.get_param() == param_negative);
	}
	SECTION("insert")
	{
		std::string insert{ "insert=12/34#test_png.png" }; 
		std::vector<std::string> param_insert{"12", "34", "test_png.png"};
		clpp::Command test_Command{insert};
		CHECK(test_Command.get_command() == clpp::CommandType::insert);
		CHECK(test_Command.get_param() == param_insert);
	}
	SECTION("convert_to")
	{
		std::string convert_to{ "convert_to=ppm" }; 
		std::vector<std::string> param_convert_to{"ppm"};
		clpp::Command test_Command{convert_to};
		CHECK(test_Command.get_command() == clpp::CommandType::convert_to);
		CHECK(test_Command.get_param() == param_convert_to);
	}
	SECTION("reflect_x")
	{
		std::string reflect_x{ "reflect_x" }; 
		std::vector<std::string> param_reflect_x{};
		clpp::Command test_Command{reflect_x};
		CHECK(test_Command.get_command() == clpp::CommandType::reflect_x);
		CHECK(test_Command.get_param() == param_reflect_x);
	}
	SECTION("reflect_y")
	{
		std::string reflect_y{ "reflect_y" }; 
		std::vector<std::string> param_reflect_y{};
		clpp::Command test_Command{reflect_y};
		CHECK(test_Command.get_command() == clpp::CommandType::reflect_y);
		CHECK(test_Command.get_param() == param_reflect_y);
	}
	SECTION("version")
	{
		std::string version{ "version" }; 
		std::vector<std::string> param_version{};
		clpp::Command test_Command{version};
		CHECK(test_Command.get_command() == clpp::CommandType::version);
		CHECK(test_Command.get_param() == param_version);
	}
	SECTION("help")
	{
		std::string help{ "help" }; 
		std::vector<std::string> param_help{};
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
