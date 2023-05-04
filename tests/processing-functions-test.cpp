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
        for(int i = 0; i < 256; ++i){
            for(int j = 0; j < 256; ++j){
                int R = test_pixel_map.at(i, j).R();
                int G = test_pixel_map.at(i, j).G();
                int B = test_pixel_map.at(i, j).B();
                REQUIRE(image.get_map().at(i, j) == img::Color (255 - R, 255 - G, 255 - B));
            }
        }
    }
    SECTION("double negative on image"){
        proc::negative(image);
        for(int i = 0; i < 256; ++i){
            for(int j = 0; j < 256; ++j){
                REQUIRE(image.get_map().at(i, j) == test_pixel_map.at(i, j));
            }
        }
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
        for(double i = 0; i <= 90; ++i){
            for(double j = 0; j <= 90; ++j){
                image.get_map() = test_pixel_map;
                int rows = image.get_map().rows();
                int columns = image.get_map().columns();
                proc::crop(image, i, j, 0, 0);
                REQUIRE(round(((100 - j)/100)*columns) * round(((100 - i)/100)*rows) == image.get_map().rows() * image.get_map().columns());
            }
        }
    }
    SECTION("checking square value (bottom and right)"){
        for(double i = 0; i <= 90; ++i){
            for(double j = 0; j <= 90; ++j){
                image.get_map() = test_pixel_map;
                int rows = image.get_map().rows();
                int columns = image.get_map().columns();
                proc::crop(image, 0, 0, j, i);
                REQUIRE(round(((100 - j)/100)*columns) * round(((100 - i)/100)*rows) == image.get_map().rows() * image.get_map().columns());
            }
        }
    }
    SECTION("checking crop from different sides"){
        for(double i = 0; i < 100; ++i){
            image.get_map() = test_pixel_map;
            proc::crop(image, i, 0, 0, 0);
            int rows = image.get_map().rows();
            int columns = image.get_map().columns();
            REQUIRE(round(1000*(100-i)/100)*100 == rows * columns);
        }
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
                image.get_map() = test_pixel_map;
                proc::insert(image, other, j, i);
                for(int y = i; y < i + 10; ++y){
                    for(int x = j; x < j + 10; ++x){
                        if(y < test_rows && x < test_columns){
                            REQUIRE(other.get_map().at(y - i, x - j) == image.get_map().at(y, x));
                        }
                    }
                }
            }
        }
    }

}

TEST_CASE("Using reflexion by x"){
    using img_t = img::PPMImage;
    img_t image;
    image.read("resources/west_1.ppm");
    img::PixelMap pixel_map = image.get_map();
    int rows = pixel_map.rows();
    int columns = pixel_map.columns();
    proc::reflex_x(image);
    img::PixelMap reflexed_pixel_map = image.get_map();

    SECTION("checking reflexion (x)"){
        for(int i = 0; i < rows; ++i){
            for(int j = 0; j < columns; ++j){
                REQUIRE(reflexed_pixel_map.at(i, j) == pixel_map.at(rows - i - 1, j));
            }
        }
    }
    SECTION("double reflexion"){
        proc::reflex_x(image);
        img::PixelMap reflexed_pixel_map = image.get_map();
        for(int i = 0; i < rows; ++i){
            for(int j = 0; j < columns; ++j){
                REQUIRE(reflexed_pixel_map.at(i, j) == pixel_map.at(i, j));
            }
        }
    }
}

TEST_CASE("Using reflexion by y"){
    using img_t = img::PPMImage;
    img_t image;
    image.read("resources/west_1.ppm");
    img::PixelMap pixel_map = image.get_map();
    int rows = pixel_map.rows();
    int columns = pixel_map.columns();
    proc::reflex_y(image);
    img::PixelMap reflexed_pixel_map = image.get_map();

    SECTION("checking reflexion (y)"){
        for(int i = 0; i < rows; ++i){
            for(int j = 0; j < columns; ++j){
                REQUIRE(reflexed_pixel_map.at(i, j) == pixel_map.at(i, columns - j - 1));
            }
        }
    }
    SECTION("double reflexion"){
        proc::reflex_y(image);
        img::PixelMap reflexed_pixel_map = image.get_map();
        for(int i = 0; i < rows; ++i){
            for(int j = 0; j < columns; ++j){
                REQUIRE(reflexed_pixel_map.at(i, j) == pixel_map.at(i, j));
            }
        }
    }
}

TEST_CASE("Resizing"){
    using img_t = img::PPMImage;
    img_t image;
    image.read("resources/west_1.ppm");
    img::PixelMap pixel_map = image.get_map();
    int rows = pixel_map.rows();
    int columns = pixel_map.columns();

    SECTION("checking size"){
        for(double k = 0.1; k <= 10; k += 0.1){
            image.get_map() = pixel_map;
            proc::resize(image, k);
            REQUIRE(image.get_map().rows() == round(rows * k));
            REQUIRE(image.get_map().columns() == round(columns * k));
        }
    }
    SECTION("checking integer k"){
        for(double k = 1; k <= 10; ++k){
            proc::resize(image, k);
            proc::resize(image, 1/k);
            for(int i = 0; i < rows; ++i){
                for(int j = 0; j < columns; ++j){
                    REQUIRE(pixel_map.at(i, j) == image.get_map().at(i, j));
                }
            }
        }
    }
}

TEST_CASE("test rotate"){
    using img_t = img::PPMImage;
    img_t image;
    image.read("resources/sign_1.ppm");
    img::PixelMap pixel_map = image.get_map();
    int rows = pixel_map.rows();
    int columns = pixel_map.columns();

    img_t img0 = image;
    img_t img90 = image;
    img_t img180 = image;
    img_t img270 = image;

    // SECTION("90x angles"){
    //     for(int i = 0; i < rows; ++i){
    //         for(int j = 0; j < columns; ++j){
    //             proc::rotate(img0, 0);
    //             proc::rotate(img90, 90);
    //             proc::rotate(img180, 180);
    //             proc::rotate(img270, 270);
    //             REQUIRE(image.get_map().at(i, j) == img0.get_map().at(i, j));
    //             std::cout << j << " " << i << std::endl;
    //             REQUIRE(image.get_map().at(i, j) == img90.get_map().at(columns - j, i));
    //             REQUIRE(image.get_map().at(i, j) == img180.get_map().at(rows - i, columns - j));
    //             REQUIRE(image.get_map().at(i, j) == img270.get_map().at(j, rows - i));
    //         }
    //     }
    // }
}
