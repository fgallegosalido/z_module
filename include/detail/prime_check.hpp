#ifndef _Z_MODULE_PRIME_CHECK_HPP_
#define _Z_MODULE_PRIME_CHECK_HPP_

#include <limits>
#include "concepts.hpp"

namespace fgs::detail{
    using namespace  concepts;
    /* Primality test for compile-time evaluation
     *
     * Source code taken from Casey's answer at Stack Overflow:
     * https://stackoverflow.com/questions/18303632/compile-time-prime-checking
     */
#ifdef FGS_PRIME_CHECK_SUPPORT
    namespace primes{
        template<Integral U>
        constexpr U mid(U low, U high) {
            return (low + high) / 2;
        }

        // precondition: low*low <= n, high*high > n.
        template<Integral U>
        constexpr U ceilsqrt (U n, U low, U high){
            return low + 1 >= high
                ? high
                : (mid(low, high) * mid(low, high) == n)
                    ? mid(low, high)
                    : (mid(low, high) * mid(low, high) <  n)
                        ? ceilsqrt(n, mid(low, high), high)
                        : ceilsqrt(n, low, mid(low, high));
        }

        // returns ceiling(sqrt(n))
        template<Integral U>
        constexpr U ceilsqrt (U n){
            return n < 3
                ? n
                    : ceilsqrt(n, U(1), U(1) << (std::numeric_limits<U>::digits / 2));
        }

        // returns true if n is divisible by an odd integer in
        // [2 * low + 1, 2 * high + 1).
        template<Integral U>
        constexpr bool find_factor (U n, U low, U high){
            return low + 1 >= high
                ? (n % (2 * low + 1)) == 0
                    : (find_factor(n, low, mid(low, high))
                        || find_factor(n, mid(low, high), high));
        }
    }  // namespace primes

    template<Integral U>
    constexpr bool is_prime (U n){
        return n > 1
            && (n == 2
                || (n % 2 == 1
                    && (n == 3
                        || !primes::find_factor(n, U(1), (primes::ceilsqrt(n) + 1) / 2))));
    }
#else
    template<Integral U>
    constexpr bool is_prime (U){
        return true;
    }
#endif
}  // namespace fgs::aux

#endif
