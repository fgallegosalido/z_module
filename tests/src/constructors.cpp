#include <catch2/catch.hpp>
#include "z_module.hpp"

#include <string>
#include <string_view>

TEST_CASE("Integral constructors"){
    fgs::Z<1237> a1{9769u};
    fgs::Z<1237> a2{-9769};
    fgs::Z<9769> a3{fgs::Z<1237>{5000}};
    fgs::Z<1237> a4{fgs::Z<9769>{3000}};

    REQUIRE(a1 == 1110);
    REQUIRE(a2 == 127);
    REQUIRE(a3 == 52);
    REQUIRE(a4 == 526);
}

TEST_CASE("String constructors"){
    fgs::Z<1237> a1{std::string{"1000000000000000000000000"}};
    fgs::Z<1237> a2{std::string_view{"1000000000000000000000000"}};
    fgs::Z<1237> a3{"1000000000000000000000000"};
    fgs::Z<1237> a4{"1000000000000000000000000", 24};

    REQUIRE(a1 == 211);
    REQUIRE(a2 == 211);
    REQUIRE(a3 == 211);
    REQUIRE(a4 == 887);
}
