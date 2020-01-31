#include <catch2/catch.hpp>
#include "z_module.hpp"

TEST_CASE("Increment"){
    fgs::Z<1237> a1{0};
    fgs::Z<1237> a2{34};
    fgs::Z<1237> a3{1236};

    SECTION("Pre-increment"){
        REQUIRE(++a1 == 1);
        REQUIRE(++a2 == 35);
        REQUIRE(++a3 == 0);
    }

    SECTION("Post-increment"){
        REQUIRE(a1++ == 0);
        REQUIRE(a2++ == 34);
        REQUIRE(a3++ == 1236);
    }
}

TEST_CASE("Decrement"){
    fgs::Z<1237> a1{0};
    fgs::Z<1237> a2{34};
    fgs::Z<1237> a3{1236};

    SECTION("Pre-decrement"){
        REQUIRE(--a1 == 1236);
        REQUIRE(--a2 == 33);
        REQUIRE(--a3 == 1235);
    }

    SECTION("Post-decrement"){
        REQUIRE(a1-- == 0);
        REQUIRE(a2-- == 34);
        REQUIRE(a3-- == 1236);
    }
}
