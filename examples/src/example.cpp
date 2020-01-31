#include "z_module.hpp"

#include <gmpxx.h>
#include <boost/multiprecision/cpp_int.hpp>

#include <iostream>

int main(){
    boost::multiprecision::cpp_int n1 = -101;
    mpz_class n2 = -101;

    fgs::Z<5> a1{n1};
    fgs::Z<5> a2{n2.get_str()};

    std::clog
        << static_cast<boost::multiprecision::cpp_int>(a1)  << '\n'
        << static_cast<mpz_class>(a2)                       << '\n';
}
