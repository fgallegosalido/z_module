#ifndef _Z_MODULE_ALGS_HPP_
#define _Z_MODULE_ALGS_HPP_

#include "z_module.hpp"

#include <utility>      // std::declval
#include <type_traits>  // std::true_type, std::false_type

namespace fgs{

using namespace concepts;

// Type trait to check if type is a Z-module
template <typename T>
struct is_z_module : std::false_type {};
template <auto Integer>
struct is_z_module<ZModule<Integer>> : std::true_type {};
template <typename T>
inline constexpr bool is_z_module_v = is_z_module<T>::value;

// Function to calculate a modular number raised to a power
// Fast exponentiation algorithm is used
template<auto Integer, Integral IntType>
constexpr ZModule<Integer> pow(const ZModule<Integer>& base, const IntType& exponent)
    noexcept(noexcept(ZModule<Integer>().operator/=(ZModule<Integer>())))
{
    if (exponent < 0)
        return pow(1/base, -exponent);
    else if (exponent == 0)
        return ZModule<Integer>(1);
    else if (exponent%2 == 0)
        return pow(base*base, exponent/2);
    else
        return pow(base, exponent-1)*base;
}

}   // namespace fgs

#endif
