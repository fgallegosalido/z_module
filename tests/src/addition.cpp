#include <catch2/catch.hpp>
#include "z_module.hpp"

TEST_CASE("Assignment addition operator +="){
    fgs::Z<1237> a{1000};

    SECTION("Operand is z-module of same cardinality"){
        fgs::Z<1237> b1{100};
        fgs::Z<1237> b2{1000};

        REQUIRE((a+=b1) == 1100);
        REQUIRE((a+=b2) == 237);
    }

    SECTION("Operand is different type"){
        fgs::Z<1237u> b1{100};
        fgs::Z<9769> b2{1000};

        REQUIRE((a+=b1) == 1100);
        REQUIRE((a+=b2) == 237);
    }
}
