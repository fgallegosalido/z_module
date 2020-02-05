#include "z_module.hpp"

#include <gmpxx.h>
#include <boost/multiprecision/cpp_int.hpp>

#include <iostream>

auto main() -> int {    // NOLINT
    boost::multiprecision::cpp_int n1 = -101;   // NOLINT
    mpz_class n2 = -101;

    fgs::Z<5> a1{n1.str()};     // NOLINT
    fgs::Z<5> a2{n2.get_str()};

    std::clog
        << static_cast<boost::multiprecision::cpp_int>(a1)  << '\n' // NOLINT
        << static_cast<mpz_class>(a2)                       << '\n';
}
