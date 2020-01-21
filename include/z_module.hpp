#ifndef Z_MODULE_HPP__
#define Z_MODULE_HPP__

#ifdef FGS_EXCEPTIONS_SUPPORT
    #undef FGS_EXCEPTIONS_SUPPORT
    #define FGS_EXCEPTIONS_SUPPORT true
#else
    #define FGS_EXCEPTIONS_SUPPORT false
#endif

#include "detail/common_type.hpp"
#include "detail/io_helper.hpp"
#include "detail/prime_check.hpp"

#include <concepts>
#include <iostream>     // std::basic_istream, std::basic_ostream
#include <numeric>      // std::gcd
#include <string>       // std::basic_string
#include <string_view>  // std::basic_string_view
#include <type_traits>  // All type traits used

// The thing is to mark some specific functions as noexcept
// depending on this macro, so we are going to define a new one
// with boolean values for that purpose.
#if FGS_EXCEPTIONS_SUPPORT
    #include <exception>    // std::domain_error
#endif

namespace fgs{

// ZModule class for integral values (cardinality bigger than 1)
template <std::integral auto Integer> requires (Integer > 1)
class ZModule{
public:
    template <std::integral auto Integer2> requires (Integer2 > 1)
    friend class ZModule;

    // Typedef for the value_type
    using value_type = std::make_unsigned_t<decltype(Integer)>;
    // Variable holding the cardinal of the ring
    static constexpr value_type N = static_cast<value_type>(Integer);
    // Class function that returns the name of this ZModule
    inline static const std::string NAME{detail::integer_set_symbol + detail::subindex_string(N)};

    // Constructors
    constexpr ZModule ()                          = default;
    constexpr ZModule (const ZModule&)            = default;
    constexpr ZModule (ZModule&&)                 = default;
    constexpr ZModule& operator= (const ZModule&) = default;
    constexpr ZModule& operator= (ZModule&&)      = default;

    // Constructor specialized for unsigned integral types
    template <std::unsigned_integral T>
    explicit constexpr ZModule (const T &other) noexcept
        : n{other % N} {}

    // Constructor specialized for signed integral types
    template <std::signed_integral T>
    explicit constexpr ZModule (const T &other) noexcept
        : n{(other < 0) ? N-(-other)%N : other%N} {}

    // Constructor specialized for z-modules of lower or equal cardinalities
    template <auto Integer2>
    requires (Integer2 < N) ||
             (Integer2 == N && !std::same_as<decltype(Integer), decltype(Integer2)>)
    explicit constexpr ZModule (const ZModule<Integer2> &other) noexcept
        : n{other.n} {}

    // Constructor specialized for z-modules of bigger cardinalities
    template <auto Integer2>
    requires (N < Integer2)
    explicit constexpr ZModule (const ZModule<Integer2> &other) noexcept
        : n{other.n % N} {}

    // Constructor using a different type. The type is required to fulfill
    // three conditions:
    //      -Implement operator< with integral types (full set of
    //      comparison operators is recommended)
    //      -Implement unary operator-
    //      -Explicitly convertible to value_type
    template <typename U>
    requires (!std::integral<U> && std::constructible_from<value_type, U>)
    explicit constexpr ZModule (const U &other) noexcept
        : n{    // This weird initilization is needed to allow constexpr
            (other < 0)
                ? N - static_cast<value_type>((other*(-1))%N)
                : static_cast<value_type>(other%N)
           }
    {}

    // Set of constructors specialized for arbitrary long numbers
    template <typename CharT, typename Traits, typename Allocator>
    explicit constexpr ZModule (const std::basic_string<CharT, Traits, Allocator> &s)
        noexcept(!FGS_EXCEPTIONS_SUPPORT)
        : n{detail::mod_aux(s.begin(), s.end(), N)}{}

    template <typename CharT, typename Traits>
    explicit constexpr ZModule (const std::basic_string_view<CharT, Traits> &s)
        noexcept(!FGS_EXCEPTIONS_SUPPORT)
        : n{detail::mod_aux(s.begin(), s.end(), N)}{}

    template <typename CharT>
    explicit constexpr ZModule (const CharT *s)
        noexcept(!FGS_EXCEPTIONS_SUPPORT)
        : ZModule{std::basic_string_view<CharT>{s}}{}

    template <typename CharT>
    constexpr ZModule (const CharT *s, std::size_t count)
        : ZModule{std::basic_string_view<CharT>{s, count}}{}

    // Increment and decrement operators
    constexpr ZModule& operator++ () noexcept{
        n = (n+1)%N;
        return *this;
    }
    constexpr ZModule& operator-- () noexcept{
        n = (n==0)?N-1:n-1;
        return *this;
    }
    constexpr ZModule operator++ (int) noexcept{
        ZModule ret{*this};
        ++(*this);
        return ret;
    }
    constexpr ZModule operator-- (int) noexcept{
        ZModule ret{*this};
        --(*this);
        return ret;
    }

    // Operator overloadings for modular arithmetic
    constexpr ZModule& operator+= (const ZModule &zm) noexcept {
        n = (n+zm.n)%N;
        return *this;
    }
    constexpr ZModule& operator-= (const ZModule &zm) noexcept {
        n = (n+(N-zm.n))%N;
        return *this;
    }
    constexpr ZModule& operator*= (const ZModule &zm) noexcept {
        n = (n*zm.n)%N;
        return *this;
    }
    constexpr ZModule& operator/= (const ZModule &zm) noexcept(!FGS_EXCEPTIONS_SUPPORT) {
        n = (n*zm.inverse())%N;
        return *this;
    }

    // Operator overloadings for compatibility with other types
    // (they all requiere to be explicitly convertible to value_type)
    template<typename U>
    requires std::constructible_from<ZModule<Integer>, U>
    constexpr ZModule& operator+= (const U &other) noexcept {
        return *this += ZModule{other};
    }
    template<typename U>
    requires std::constructible_from<ZModule<Integer>, U>
    constexpr ZModule& operator-= (const U &other) noexcept {
        return *this -= ZModule{other};
    }
    template<typename U>
    requires std::constructible_from<ZModule<Integer>, U>
    constexpr ZModule& operator*= (const U &other) noexcept {
        return *this *= ZModule{other};
    }
    template<typename U>
    requires std::constructible_from<ZModule<Integer>, U>
    constexpr ZModule& operator/= (const U &other) noexcept(!FGS_EXCEPTIONS_SUPPORT) {
        return *this /= ZModule{other};
    }

    // Explicit conversion to any type explicitly convertible to
    // the underlined value_type
    template<std::constructible_from<value_type> T>
    constexpr explicit operator T() const noexcept {
        return static_cast<T>(n);
    }

    // Explicit conversion to another integer ring
    template<auto Integer2>
    constexpr explicit operator ZModule<Integer2>() const noexcept {
        if constexpr(Integer <= Integer2)
            return ZModule<Integer2>(*this);
        else
            return ZModule<Integer2>(n%Integer2);
    }

    // I/O overloadings for ZModule objects
    // It accepts arbitrary long inputs
    template <typename CharT, typename Traits>
    friend std::basic_istream<CharT, Traits>&
    operator>> (std::basic_istream<CharT, Traits> &is, ZModule &zm){
        // We first need to take the input in a string to parse it
        std::string input; is >> input;
        zm.n = detail::mod_aux(input.begin(), input.end(), N);

        return is;
    }

    template <typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits>&
    operator<< (std::basic_ostream<CharT, Traits> &os, const ZModule &zm){
        os << zm.n;
        return os;
    }

private:
    value_type n;

    // Calculate the inverse of the number in the ring
    constexpr auto inverse() const noexcept(!FGS_EXCEPTIONS_SUPPORT){
        // If this macro is not defined, impossible operations
        // will be left as undefined behaviour instead of throwing
#if FGS_EXCEPTIONS_SUPPORT
        if (n == 0)
            throw std::domain_error("Divide by zero exception");

        // If N is prime, there's no doubts that gcd(N, n) = 1, so we can
        // avoid that computation by dispatching at compile time.
        //
        // If the user wants this feature, this macro has to be defined.
        // Otherwise, gcd(N, n) has to be computed, no matter if N is prime or not
#ifdef FGS_PRIME_CHECK_SUPPORT
        if constexpr (!detail::is_prime(N))
#endif
            if (std::gcd(N, n) != 1)
                throw std::domain_error(std::to_string(n) + " has no inverse in " + NAME);
#endif

        // TODO: Implement Extended Euclidean Algorithm, this one is slow
        // TODO: Use a constexpr pattern to force compile-time evaluation
        value_type ret=1;
        while ((n*ret)%N != 1)
            ++ret;
        return ret;
    }
};

// Unary + and - operators
template<auto Integer>
constexpr ZModule<Integer> operator+ (const ZModule<Integer> &rhs) noexcept {
    return ZModule<Integer>(rhs);
}
template<auto Integer>
constexpr ZModule<Integer> operator- (const ZModule<Integer> &rhs) noexcept {
    return ZModule<Integer>(0u) -= rhs;
}

// Binary +, - and * operators for same cardinality
template<auto Integer1, auto Integer2>
requires (Integer1 == Integer2)
constexpr auto operator+ (const ZModule<Integer1> &lhs,
                          const ZModule<Integer2> &rhs) noexcept {
    return std::common_type_t<ZModule<Integer1>, ZModule<Integer2>>(lhs) += rhs;
}
template<auto Integer1, auto Integer2>
requires (Integer1 == Integer2)
constexpr auto operator- (const ZModule<Integer1> &lhs,
                          const ZModule<Integer2> &rhs) noexcept {
    return std::common_type_t<ZModule<Integer1>, ZModule<Integer2>>(lhs) -= rhs;
}
template<auto Integer1, auto Integer2>
requires (Integer1 == Integer2)
constexpr auto operator* (const ZModule<Integer1> &lhs,
                          const ZModule<Integer2> &rhs) noexcept {
    return std::common_type_t<ZModule<Integer1>, ZModule<Integer2>>(lhs) *= rhs;
}
template<auto Integer1, auto Integer2>
requires (Integer1 == Integer2)
constexpr auto operator/ (const ZModule<Integer1> &lhs,
                          const ZModule<Integer2> &rhs) noexcept(!FGS_EXCEPTIONS_SUPPORT) {
    return std::common_type_t<ZModule<Integer1>, ZModule<Integer2>>(lhs) /= rhs;
}


/*****************************************************************************/
/*************** Binary operators +, -, * for different types ****************/
/*****************************************************************************/
template<auto Integer, typename U>
constexpr ZModule<Integer> operator+ (const ZModule<Integer> &lhs, const U &rhs) noexcept {
   return ZModule<Integer>(lhs) += rhs;
}
template<auto Integer, typename U>
constexpr ZModule<Integer> operator- (const ZModule<Integer> &lhs, const U &rhs) noexcept {
   return ZModule<Integer>(lhs) -= rhs;
}
template<auto Integer, typename U>
constexpr ZModule<Integer> operator* (const ZModule<Integer> &lhs, const U &rhs) noexcept {
   return ZModule<Integer>(lhs) *= rhs;
}
template<auto Integer, typename U>
constexpr ZModule<Integer> operator/ (const ZModule<Integer> &lhs, const U &rhs) noexcept(!FGS_EXCEPTIONS_SUPPORT) {
   return ZModule<Integer>(lhs) /= rhs;
}
/*------------------------------------------*/
template<auto Integer, typename U>
constexpr ZModule<Integer> operator+ (const U &lhs, const ZModule<Integer> &rhs) noexcept {
   return ZModule<Integer>(lhs) += rhs;
}
template<auto Integer, typename U>
constexpr ZModule<Integer> operator- (const U &lhs, const ZModule<Integer> &rhs) noexcept {
   return ZModule<Integer>(lhs) -= rhs;
}
template<auto Integer, typename U>
constexpr ZModule<Integer> operator* (const U &lhs, const ZModule<Integer> &rhs) noexcept {
   return ZModule<Integer>(lhs) *= rhs;
}
template<auto Integer, typename U>
constexpr ZModule<Integer> operator/ (const U &lhs, const ZModule<Integer> &rhs) noexcept(!FGS_EXCEPTIONS_SUPPORT) {
   return ZModule<Integer>(lhs) /= rhs;
}

// This class is not meant to be used for bitwise operations, so we
// give the ^ operator a new meaning (pow basically)
template<auto Integer>
constexpr ZModule<Integer> operator^ (const ZModule<Integer> &base, const std::integral auto &exponent)
    noexcept(!FGS_EXCEPTIONS_SUPPORT)
{
    if (exponent < 0)
        return (1/base) ^ (-exponent);
    else if (exponent == 0)
        return ZModule<Integer>(1);
    else if (exponent%2 == 0)
        return (base*base) ^ (exponent/2);
    else
        return (base ^ (exponent-1))*base;
}


// Operator overloadings for comparisons between z-modules of same cardinality
template<auto Integer1, auto Integer2> requires (Integer1 == Integer2)
constexpr bool operator== ( const ZModule<Integer1> &lhs,
                            const ZModule<Integer2> &rhs) noexcept
{
    return static_cast<typename ZModule<Integer1>::value_type>(lhs) ==
            static_cast<typename ZModule<Integer2>::value_type>(rhs);
}
template<auto Integer1, auto Integer2> requires (Integer1 == Integer2)
constexpr bool operator!= ( const ZModule<Integer1> &lhs,
                            const ZModule<Integer2> &rhs) noexcept
{
    return static_cast<typename ZModule<Integer1>::value_type>(lhs) !=
            static_cast<typename ZModule<Integer2>::value_type>(rhs);
}
template<auto Integer1, auto Integer2> requires (Integer1 == Integer2)
constexpr bool operator<  ( const ZModule<Integer1> &lhs,
                            const ZModule<Integer2> &rhs) noexcept
{
    return static_cast<typename ZModule<Integer1>::value_type>(lhs) <
            static_cast<typename ZModule<Integer2>::value_type>(rhs);
}
template<auto Integer1, auto Integer2> requires (Integer1 == Integer2)
constexpr bool operator<= ( const ZModule<Integer1> &lhs,
                            const ZModule<Integer2> &rhs) noexcept
{
    return static_cast<typename ZModule<Integer1>::value_type>(lhs) <=
            static_cast<typename ZModule<Integer2>::value_type>(rhs);
}
template<auto Integer1, auto Integer2> requires (Integer1 == Integer2)
constexpr bool operator>  ( const ZModule<Integer1> &lhs,
                            const ZModule<Integer2> &rhs) noexcept
{
    return static_cast<typename ZModule<Integer1>::value_type>(lhs) >
            static_cast<typename ZModule<Integer2>::value_type>(rhs);
}
template<auto Integer1, auto Integer2> requires (Integer1 == Integer2)
constexpr bool operator>= ( const ZModule<Integer1> &lhs,
                            const ZModule<Integer2> &rhs) noexcept
{
    return static_cast<typename ZModule<Integer1>::value_type>(lhs) >=
            static_cast<typename ZModule<Integer2>::value_type>(rhs);
}

// Operator overloadings for comparisons with other types
template<auto Integer, typename U>
constexpr bool operator== (const ZModule<Integer> &lhs, const U &rhs) noexcept {
   return lhs == ZModule<Integer>(rhs);
}
template<auto Integer, typename U>
constexpr bool operator!= (const ZModule<Integer> &lhs, const U &rhs) noexcept {
   return lhs != ZModule<Integer>(rhs);
}
template<auto Integer, typename U>
constexpr bool operator<  (const ZModule<Integer> &lhs, const U &rhs) noexcept {
   return lhs <  ZModule<Integer>(rhs);
}
template<auto Integer, typename U>
constexpr bool operator<= (const ZModule<Integer> &lhs, const U &rhs) noexcept {
   return lhs <= ZModule<Integer>(rhs);
}
template<auto Integer, typename U>
constexpr bool operator>  (const ZModule<Integer> &lhs, const U &rhs) noexcept {
   return lhs >  ZModule<Integer>(rhs);
}
template<auto Integer, typename U>
constexpr bool operator>= (const ZModule<Integer> &lhs, const U &rhs) noexcept {
   return lhs >= ZModule<Integer>(rhs);
}
/*------------------------------------------*/
template<auto Integer, typename U>
constexpr bool operator== (const U &lhs, const ZModule<Integer> &rhs) noexcept {
   return ZModule<Integer>(lhs) == rhs;
}
template<auto Integer, typename U>
constexpr bool operator!= (const U &lhs, const ZModule<Integer> &rhs) noexcept {
   return ZModule<Integer>(lhs) != rhs;
}
template<auto Integer, typename U>
constexpr bool operator<  (const U &lhs, const ZModule<Integer> &rhs) noexcept {
   return ZModule<Integer>(lhs) <  rhs;
}
template<auto Integer, typename U>
constexpr bool operator<= (const U &lhs, const ZModule<Integer> &rhs) noexcept {
   return ZModule<Integer>(lhs) <= rhs;
}
template<auto Integer, typename U>
constexpr bool operator>  (const U &lhs, const ZModule<Integer> &rhs) noexcept {
   return ZModule<Integer>(lhs) >  rhs;
}
template<auto Integer, typename U>
constexpr bool operator>= (const U &lhs, const ZModule<Integer> &rhs) noexcept {
   return ZModule<Integer>(lhs) >= rhs;
}

// Type trait to check if some type is a Z-module
template <typename T>
inline constexpr bool is_z_module = false;
template <auto Integer>
inline constexpr bool is_z_module<ZModule<Integer>> = true;

// We can't achive the good mathematical notation for the type, but we
// can provide a similar one (Z<n>)
template<auto Integer>
using Z = ZModule<Integer>;

}   // namespace fgs

#endif
