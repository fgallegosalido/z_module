#ifndef _Z_MODULE_PRIME_CHECK_HPP_
#define _Z_MODULE_PRIME_CHECK_HPP_

#include <array>
#include <limits>

namespace fgs::detail{

    // Compile-time primality test "highly optimized" to accept large inputs
    consteval bool is_prime(std::integral auto N){
        if (N < 2)
            return false;

        using value_type = decltype(N);

        // Base cases to create the sieve
        constexpr std::array base_primes = { // <-- Deduction guides in action
            value_type(2),
            value_type(3),
            value_type(5),
            value_type(7),
        };
        value_type next_prime = 11;
        value_type increment = 1;

        // We just test against the base primes
        for (auto const &i : base_primes){
            if (N == i)   return true;
            if (N%i == 0) return false;

            // step size for the main loop
            increment *= i;
        }

        // Main loop. Since I can't generate the cases, they're handwritten
        for (value_type i=next_prime; i*i < N; i += increment){
            if (N%(i+0)  == 0) return false;
            if (N%(i+2)  == 0) return false;
            if (N%(i+6)  == 0) return false;
            if (N%(i+8)  == 0) return false;
            if (N%(i+12) == 0) return false;
            if (N%(i+18) == 0) return false;
            if (N%(i+20) == 0) return false;
            if (N%(i+26) == 0) return false;
            if (N%(i+30) == 0) return false;
            if (N%(i+32) == 0) return false;
            if (N%(i+36) == 0) return false;
            if (N%(i+42) == 0) return false;
            if (N%(i+48) == 0) return false;
            if (N%(i+50) == 0) return false;
            if (N%(i+56) == 0) return false;
            if (N%(i+60) == 0) return false;
            if (N%(i+62) == 0) return false;
            if (N%(i+68) == 0) return false;
            if (N%(i+72) == 0) return false;
            if (N%(i+78) == 0) return false;
            if (N%(i+86) == 0) return false;
            if (N%(i+90) == 0) return false;
            if (N%(i+92) == 0) return false;
            if (N%(i+96) == 0) return false;
            if (N%(i+98) == 0) return false;
            if (N%(i+102) == 0) return false;
            if (N%(i+110) == 0) return false;
            if (N%(i+116) == 0) return false;
            if (N%(i+120) == 0) return false;
            if (N%(i+126) == 0) return false;
            if (N%(i+128) == 0) return false;
            if (N%(i+132) == 0) return false;
            if (N%(i+138) == 0) return false;
            if (N%(i+140) == 0) return false;
            if (N%(i+146) == 0) return false;
            if (N%(i+152) == 0) return false;
            if (N%(i+156) == 0) return false;
            if (N%(i+158) == 0) return false;
            if (N%(i+162) == 0) return false;
            if (N%(i+168) == 0) return false;
            if (N%(i+170) == 0) return false;
            if (N%(i+176) == 0) return false;
            if (N%(i+180) == 0) return false;
            if (N%(i+182) == 0) return false;
            if (N%(i+186) == 0) return false;
            if (N%(i+188) == 0) return false;
            if (N%(i+198) == 0) return false;
            if (N%(i+200) == 0) return false;
        }

        return true;
    }
}  // namespace fgs::detail

#endif
