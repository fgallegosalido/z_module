#include "z_module.hpp"
#include "z_module_algs.hpp"

#include <iostream>
#include <sstream>
#include <utility>

// Function to check correctness of ZModule class
template<auto UInt1, auto UInt2>
void test_z_module(){
    using namespace fgs;
    // Name of the class
    std::clog << "The set of integers module " << UInt1 << " is called " << Z<UInt1>::NAME << "\n";

    // Check all constructors and assingments
    Z<UInt1> cd, cn(10), cc(cn), cm(std::move(cc));
    cd = cn;
    cd = std::move(cm);


    // Construction
    Z<UInt1> a(10), b(20), c(30);

    // Increment and decrement
    ++a;  --a;
    a++;  a--;

    // Unary + and -
    +a; -a;

    // Arithmetic for same type +=, -=, *= and /=
    a += b;
    a -= b;
    a *= b;
    a /= b;

    // Arithmetic for PODs types +=, -=, *= and /=
    a += 5;
    a -= 5;
    a *= 5;
    a /= 5;

    // Binary +, - and * operators for same type
    a = b + c;
    a = b - c;
    a = b * c;
    a = b / c;

    // Binary +, - and * operators for different types
    a = b + 10; a = 10 + b;
    a = b - 5;  a = 5 - b;
    a = b * 2;  a = 2 * b;
    a = b / 20;  a = 20 / b;

    // Operator ^ for pow
    a^10;
    a^-10;

    // Comparison for same types ==, !=, <, <=, > and >=
    a == b;
    a != b;
    a < b;
    a <= b;
    a > b;
    a >= b;

    // Comparison for different types ==, !=, <, <=, > and >=
    a == 10; 10 == a;
    a != 10; 10 != a;
    a  < 10; 10  < a;
    a <= 10; 10 <= a;
    a  > 10; 10  > a;
    a >= 10; 10 >= a;

    // Constructor with the value of other ring
    Z<UInt2> d(a);
    // Casting
    static_cast<Z<UInt2>>(a);

    // I/O
    Z<UInt2> e;

    std::clog << "Testing input....\n";
    std::stringstream{"628920810681886168186108890"} >> e;
    std::clog << "The typed number is " << e << " in " << Z<UInt2>::NAME << "\n";

    // Testing the traits
    if constexpr (is_z_module_v<Z<UInt1>>)
        std::clog << Z<UInt1>::NAME << " is a Z-module\n";
}

int main(){
    test_z_module<1237, 9769>();
}
