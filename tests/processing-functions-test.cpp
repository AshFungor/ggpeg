#include <catch2/catch_all.hpp>
#include <iostream>
#include <image/image.hpp>
#include <processing/processing.hpp>

TEST_CASE("Using negative filter"){
    using img_t = img::PPMImage;
    img_t image;
    img::PixelMap test_pixel_map(256, 256);
    for(int i = 0; i < 256; ++i){
        for(int j = 0; j < 256; ++j){
            test_pixel_map.at(i, j) = img::Color(i, j, abs(i - j));
        }
    }
    image.get_map() = test_pixel_map;
    proc::negative(image);

    SECTION("negative image"){
        bool check_negative = true;
        for(int i = 0; i < 256; ++i){
            for(int j = 0; j < 256; ++j){
                int R = test_pixel_map.at(i, j).R();
                int G = test_pixel_map.at(i, j).G();
                int B = test_pixel_map.at(i, j).B();
                check_negative &= image.get_map().at(i, j) == img::Color (255 - R, 255 - G, 255 - B);
            }
        }
        REQUIRE(check_negative);
    }
    SECTION("double negative on image"){
        proc::negative(image);
        bool check_double_negative = true;
        for(int i = 0; i < 256; ++i){
            for(int j = 0; j < 256; ++j){
                check_double_negative &= image.get_map().at(i, j) == test_pixel_map.at(i, j);
            }
        }
        REQUIRE(check_double_negative);
    }
}

TEST_CASE("Using crop"){
    using img_t = img::PPMImage;
    img_t image;
    img::PixelMap test_pixel_map(1000, 100);
    for(int i = 0; i < 100; ++i){
        for(int j = 0; j < 1000; ++j){
            test_pixel_map.at(i, j) = img::Color(i, j, abs(i - j));
        }
    }
    image.get_map() = test_pixel_map;

    SECTION("checking square value (top and left)"){
        bool check_square_value = true;
        for(double i = 0; i <= 90; ++i){
            for(double j = 0; j <= 90; ++j){
                image.get_map() = test_pixel_map;
                int rows = image.get_map().rows();
                int columns = image.get_map().columns();
                proc::crop(image, i, j, 0, 0);
                check_square_value &= round(((100 - j)/100)*columns) * round(((100 - i)/100)*rows) == image.get_map().rows() * image.get_map().columns();
            }
        }
        REQUIRE(check_square_value);
    }
    SECTION("checking square value (bottom and right)"){
        bool check_square_value = true;
        for(double i = 0; i <= 90; ++i){
            for(double j = 0; j <= 90; ++j){
                image.get_map() = test_pixel_map;
                int rows = image.get_map().rows();
                int columns = image.get_map().columns();
                proc::crop(image, 0, 0, j, i);
                check_square_value &= round(((100 - j)/100)*columns) * round(((100 - i)/100)*rows) == image.get_map().rows() * image.get_map().columns();
            }
        }
        REQUIRE(check_square_value);
    }
    SECTION("checking crop from different sides"){
        bool check_different_sides = true;
        for(double i = 0; i < 100; ++i){
            image.get_map() = test_pixel_map;
            proc::crop(image, i, 0, 0, 0);
            int rows = image.get_map().rows();
            int columns = image.get_map().columns();
            check_different_sides &= round(1000*(100-i)/100)*100 == rows * columns;
        }
        REQUIRE(check_different_sides);
    }
}

TEST_CASE("Using insert"){
    using img_t = img::PPMImage;
    img_t image;
    img::PixelMap test_pixel_map(200, 100);
    int test_columns = test_pixel_map.columns();
    int test_rows = test_pixel_map.rows();
    for(int i = 0; i < 100; ++i){
        for(int j = 0; j < 200; ++j){
            test_pixel_map.at(i, j) = img::Color(i % 256, j % 256, abs(i - j) % 256);
        }
    }

    img_t other;
    img::PixelMap insert_pixel_map(10, 10);
    for(int i = 0; i < 10; ++i){
        for(int j = 0; j < 10; ++j){
            insert_pixel_map.at(i, j) = img::Color(255 - i, 255 - j, abs(256 - i - j));
        }
    }
    other.get_map() = insert_pixel_map;

    SECTION("insert everywhere"){
        for(int i = 0; i < test_rows; ++i){
            for(int j = 0; j < test_columns; ++j){
                bool check_insert = true;
                image.get_map() = test_pixel_map;
                proc::insert(image, other, j, i);
                for(int y = i; y < i + 10; ++y){
                    for(int x = j; x < j + 10; ++x){
                        if(y < test_rows && x < test_columns){
                            check_insert &= other.get_map().at(y - i, x - j) == image.get_map().at(y, x);
                        }
                    }
                }
                REQUIRE(check_insert);
            }
        }
    }

}

TEST_CASE("Using reflection by x"){
    using img_t = img::PPMImage;
    img_t image;
    image.read("resources/west.ppm");
    img::PixelMap pixel_map = image.get_map();
    int rows = pixel_map.rows();
    int columns = pixel_map.columns();
    proc::reflect_x(image);
    img::PixelMap reflected_pixel_map = image.get_map();

    SECTION("checking reflection (x)"){
        bool check_reflection_x = true;
        for(int i = 0; i < rows; ++i){
            for(int j = 0; j < columns; ++j){
                check_reflection_x &= reflected_pixel_map.at(i, j) == pixel_map.at(rows - i - 1, j);
            }
        }
        REQUIRE(check_reflection_x);

    }
    SECTION("double reflection"){
        proc::reflect_x(image);
        img::PixelMap reflected_pixel_map = image.get_map();
        bool check_double_reflection_x = true;
        for(int i = 0; i < rows; ++i){
            for(int j = 0; j < columns; ++j){
                check_double_reflection_x &= reflected_pixel_map.at(i, j) == pixel_map.at(i, j);
            }
        }
        REQUIRE(check_double_reflection_x);
    }
}

TEST_CASE("Using reflection by y"){
    using img_t = img::PPMImage;
    img_t image;
    image.read("resources/west.ppm");
    img::PixelMap pixel_map = image.get_map();
    int rows = pixel_map.rows();
    int columns = pixel_map.columns();
    proc::reflect_y(image);
    img::PixelMap reflected_pixel_map = image.get_map();

    SECTION("checking reflection (y)"){
        bool check_reflection_y = true;
        for(int i = 0; i < rows; ++i){
            for(int j = 0; j < columns; ++j){
                check_reflection_y &= reflected_pixel_map.at(i, j) == pixel_map.at(i, columns - j - 1);
            }
        }
        REQUIRE(check_reflection_y);
    }
    SECTION("double reflection"){
        proc::reflect_y(image);
        img::PixelMap reflected_pixel_map = image.get_map();
        bool check_double_reflection_y = true;
        for(int i = 0; i < rows; ++i){
            for(int j = 0; j < columns; ++j){
                check_double_reflection_y &= reflected_pixel_map.at(i, j) == pixel_map.at(i, j);
            }
        }
        REQUIRE(check_double_reflection_y);
    }
}

TEST_CASE("Resizing"){
    using img_t = img::PPMImage;
    img_t image;
    image.read("resources/west.ppm");
    img::PixelMap pixel_map = image.get_map();
    int rows = pixel_map.rows();
    int columns = pixel_map.columns();
    
    SECTION("checking size"){
        for(double k = 0.1; k <= 10; k += 0.1){
            bool check_square = true;
            image.get_map() = pixel_map;
            proc::resize(image, k);
            check_square &= image.get_map().rows() == round(rows * k);
            check_square &= image.get_map().columns() == round(columns * k);
            REQUIRE(check_square);
        }
    }
    SECTION("checking integer k"){
        for(double k = 1; k <= 10; ++k){
            bool check_integer = true;
            proc::resize(image, k);
            proc::resize(image, 1/k);
            for(int i = 0; i < rows; ++i){
                for(int j = 0; j < columns; ++j){
                    check_integer &= pixel_map.at(i, j) == image.get_map().at(i, j);
                }
            }
            REQUIRE(check_integer);
        }
    }
}

TEST_CASE("test rotate"){
    using img_t = img::PPMImage;
    img_t image;
    image.read("resources/west.ppm");
    img::PixelMap pixel_map = image.get_map();
    int rows = pixel_map.rows();
    int columns = pixel_map.columns();

    SECTION("90x angles"){
        int counter0 = 0;
        int counter90 = 0;
        int counter180 = 0;
        int counter270 = 0;
        img_t img0 = image;
        img_t img90 = image;
        img_t img180 = image;
        img_t img270 = image;
        proc::rotate(img0, 0);
        proc::rotate(img90, 90);
        proc::rotate(img180, 180);
        proc::rotate(img270, 270);

        for(int i = 0; i < rows; ++i){
            for(int j = 0; j < columns; ++j){
                if(image.get_map().at(i, j) == img0.get_map().at(i, j)){
                    ++counter0;
                }
                if(image.get_map().at(i, j) == img90.get_map().at(columns - j - 1, i)){
                    ++counter90;
                }
                if(image.get_map().at(i, j) == img180.get_map().at(rows - i - 1, columns - j - 1)){
                    ++counter180;
                }
                if(image.get_map().at(i, j) == img270.get_map().at(j, rows - i - 1)){
                    ++counter270;
                }
            }
        }
        REQUIRE(counter0 == rows*columns);
        REQUIRE(counter90 == rows*columns);
        REQUIRE(counter180 == rows*columns);
        REQUIRE(counter270 == rows*columns);
    }
    SECTION("the same modulo 360"){
        for(double degrees = 0; degrees < 15; ++degrees){
            img_t img0 = image;
            img_t img_360 = image;
            img_t img360 = image;
            proc::rotate(img0, degrees);
            proc::rotate(img_360, degrees - 360);
            proc::rotate(img360, degrees + 360);
            rows = img0.get_map().rows();
            columns = img0.get_map().columns();
            bool check_360 = true;
            bool check360 = true;
            for(int i = 0; i < rows; ++i){
                for(int j = 0; j < columns; ++j){
                    check_360 &= img0.get_map().at(i, j) == img_360.get_map().at(i, j);
                    check360 &= img0.get_map().at(i, j) == img360.get_map().at(i, j);
                }
            }
            REQUIRE(check_360);
            REQUIRE(check360);
        }
    }
}
