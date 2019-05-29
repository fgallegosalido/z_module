#ifndef _Z_MODULE_HPP_
#define _Z_MODULE_HPP_

// The thing is to mark some specific functions as noexcept
// depending on this macro, so we are going to define a new one
// with boolean values for that purpose.
#ifdef FGS_EXCEPTIONS_SUPPORT
    #define FGS_EXCEPTIONS_ENABLED true
    #include <exception>    // std::domain_error
#else
    #define FGS_EXCEPTIONS_ENABLED false
#endif

#include <iostream>     // std::istream, std::ostream
#include <string>       // std::string
#include <type_traits>  // All type traits used
#include <numeric>      // std::gcd
#include <array>

#include "detail/io_helper.hpp"
#include "detail/prime_check.hpp"
#include "detail/concepts.hpp"

namespace fgs{

using namespace concepts;

template <auto Integer>
requires Integral<decltype(Integer)> // We force an Integral to be passed
class ZModule{
    // It makes no sense to have a cardinality lower than 2,
    // so we will force it to be bigger than 1.
    static_assert(Integer > 1, "Cardinal of Z-module must be bigger than 1");

public:
    template <auto Integer2>
    friend class ZModule;

    // Typedef for the value_type
    using value_type = std::make_unsigned_t<decltype(Integer)>;
    // Variable holding the cardinal of the ring
    static constexpr value_type N = static_cast<value_type>(Integer);
    // Variable holding the Euler function evaluated in N
    static constexpr value_type E = [](){
        value_type count = 1;

        for (value_type i=2; i<N; ++i)
            if (std::gcd(N, i)==1)
                ++count;

        return count;
    }();

    // Class function that returns the name of this ZModule
    static const std::string NAME;

    // Returns an array corresponding to the multiplicative subgroup
    // (all numbers n less than N such gcd(N, n) = 1)
    static constexpr auto multiplicative_subgroup(){
        std::array<value_type, E> ret;

        value_type count = 1;
        typename decltype(ret)::size_type pos = 0;

        while (pos != E){
            if (std::gcd(N, count)==1){
                ret[pos] = count;
                ++pos;
            }
            ++count;
        }

        return ret;
    }

    // Constructors
    constexpr ZModule ()                          noexcept = default;
    constexpr ZModule (const ZModule&)            noexcept = default;
    constexpr ZModule (ZModule&&)                 noexcept = default;
    constexpr ZModule& operator= (const ZModule&) noexcept = default;
    constexpr ZModule& operator= (ZModule&&)      noexcept = default;

    // Constructor specialized for unsigned integral types
    template <UnsignedIntegral T>
    explicit constexpr ZModule (const T &other) noexcept
        : n{other % N} {}

    // Constructor specialized for signed integral types
    template <SignedIntegral T>
    explicit constexpr ZModule (const T &other) noexcept
        : n{(other < 0) ? other%N+N : other%N} {}

    // Constructor specialized for z-modules of other cardinalities
    template <auto Integer2>
    explicit constexpr ZModule (const ZModule<Integer2> &other) noexcept
        : n{other.n % N} {}

    // Constructor using a different type. We require the type
    // to be, at least, explicitly convertible to value_type
    template <typename U>
    requires Constructible<value_type, U> && !Integral<U>
    explicit constexpr ZModule (const U &other) noexcept
        : n{    // This weird initilization is needed to allow constexpr
            (other < 0)
                ? N - (static_cast<value_type>(-other))%N
                : static_cast<value_type>(other)%N
           }
    {}

    template <typename CharT, typename Traits, typename Allocator>
    explicit ZModule (const std::basic_string<CharT, Traits, Allocator> &s)
        noexcept(!FGS_EXCEPTIONS_ENABLED)
        : n{detail::mod_aux(s.begin(), s.end(), N)}{}

    template <typename CharT, typename Traits>
    explicit constexpr ZModule (const std::basic_string_view<CharT, Traits> &s)
        noexcept(!FGS_EXCEPTIONS_ENABLED)
        : n{detail::mod_aux(s.begin(), s.end(), N)}{}

    template <typename CharT>
    explicit constexpr ZModule (const CharT *s)
        noexcept(!FGS_EXCEPTIONS_ENABLED)
        : ZModule{std::basic_string_view<CharT>{s}}{}

    template <typename CharT>
    constexpr ZModule (const CharT *s, std::size_t count)
        noexcept(!FGS_EXCEPTIONS_ENABLED)
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
        ZModule ret(*this);
        ++(*this);
        return ret;
    }
    constexpr ZModule operator-- (int) noexcept{
        ZModule ret(*this);
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
    constexpr ZModule& operator/= (const ZModule &zm) noexcept(!FGS_EXCEPTIONS_ENABLED) {
        n = (n*zm.inverse())%N;
        return *this;
    }

    // Operator overloadings for compatibility with other types
    // (they all requiere to be explicitly convertible to value_type)
    template<typename U>
    requires Constructible<ZModule<Integer>, U>
    constexpr ZModule& operator+= (const U &other) noexcept {
        return *this += ZModule{other};
    }
    template<typename U>
    requires Constructible<ZModule<Integer>, U>
    constexpr ZModule& operator-= (const U &other) noexcept {
        return *this -= ZModule{other};
    }
    template<typename U>
    requires Constructible<ZModule<Integer>, U>
    constexpr ZModule& operator*= (const U &other) noexcept {
        return *this *= ZModule{other};
    }
    template<typename U>
    requires Constructible<ZModule<Integer>, U>
    constexpr ZModule& operator/= (const U &other) noexcept(!FGS_EXCEPTIONS_ENABLED) {
        return *this /= ZModule{other};
    }

    // Explicit conversion to the underlined value_type
    constexpr operator value_type() const noexcept {
        return value_type(n);
    }
    // Conversion to another integer ring (must use static_cast<>())
    template<auto Integer2>
    constexpr explicit operator ZModule<Integer2>() const noexcept {
        if constexpr(Integer == Integer2)
            return ZModule<Integer2>(*this);
        else
            return ZModule<Integer2>(n%Integer2);
    }

    // I/O overloadings for ZModule objects
    // TODO: Implement an algorithm that parses arbitrary large numbers
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
    constexpr auto inverse() const noexcept(!FGS_EXCEPTIONS_ENABLED){
        // If this macro is not defined, impossible operations
        // will be left as undefined behaviour
#ifdef FGS_EXCEPTIONS_SUPPORT
        if (n == 0)
            throw std::domain_error("Divide by zero exception");
        // Check if N is prime at compile time (can be dissabled) and
        // throw an exception if there's no inverse (if the user requires it)
        if constexpr (!detail::is_prime(N))
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

// Here we initialize the NAME of the ZModule
template <auto Integer>
const std::string ZModule<Integer>::NAME{
    detail::integer_set_symbol + detail::subindex_string(N)};

// Here we initialize the Euler number of the ZModule
// We do it by inmediatly calling a lambda that calculates it
/*template <auto Integer>
typename ZModule<Integer>::value_type ZModule<Integer>::E = [](){
    value_type count = 1;

    for (value_type i=2; i<N; ++i)
        if (std::gcd(N, i)==1)
            ++count;

    return count;
}();*/

// Unary + and - operators
template<auto Integer>
constexpr ZModule<Integer> operator+ (const ZModule<Integer> &rhs) noexcept {
    return ZModule<Integer>(rhs);
}
template<auto Integer>
constexpr ZModule<Integer> operator- (const ZModule<Integer> &rhs) noexcept {
    return ZModule<Integer>(0) -= rhs;
}

// Binary +, - and * operators for same type
template<auto Integer>
constexpr ZModule<Integer> operator+ ( const ZModule<Integer> &lhs,
                                        const ZModule<Integer> &rhs) noexcept {
    return ZModule<Integer>(lhs) += rhs;
}
template<auto Integer>
constexpr ZModule<Integer> operator- ( const ZModule<Integer> &lhs,
                                        const ZModule<Integer> &rhs) noexcept {
    return ZModule<Integer>(lhs) -= rhs;
}
template<auto Integer>
constexpr ZModule<Integer> operator* ( const ZModule<Integer> &lhs,
                                        const ZModule<Integer> &rhs) noexcept {
    return ZModule<Integer>(lhs) *= rhs;
}
template<auto Integer>
constexpr ZModule<Integer> operator/ ( const ZModule<Integer> &lhs,
                                        const ZModule<Integer> &rhs) noexcept(!FGS_EXCEPTIONS_ENABLED) {
    return ZModule<Integer>(lhs) /= rhs;
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
constexpr ZModule<Integer> operator/ (const ZModule<Integer> &lhs, const U &rhs) noexcept(!FGS_EXCEPTIONS_ENABLED) {
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
constexpr ZModule<Integer> operator/ (const U &lhs, const ZModule<Integer> &rhs) noexcept(!FGS_EXCEPTIONS_ENABLED) {
   return ZModule<Integer>(lhs) /= rhs;
}


// Operator overloadings for comparisons of the same type
template<auto Integer>
constexpr bool operator== ( const ZModule<Integer> &lhs,
                            const ZModule<Integer> &rhs) noexcept
{
   return static_cast<typename ZModule<Integer>::value_type>(lhs) ==
            static_cast<typename ZModule<Integer>::value_type>(rhs);
}
template<auto Integer>
constexpr bool operator!= ( const ZModule<Integer> &lhs,
                            const ZModule<Integer> &rhs) noexcept
{
   return static_cast<typename ZModule<Integer>::value_type>(lhs) !=
            static_cast<typename ZModule<Integer>::value_type>(rhs);
}
template<auto Integer>
constexpr bool operator<  ( const ZModule<Integer> &lhs,
                            const ZModule<Integer> &rhs) noexcept
{
   return static_cast<typename ZModule<Integer>::value_type>(lhs) <
            static_cast<typename ZModule<Integer>::value_type>(rhs);
}
template<auto Integer>
constexpr bool operator<= ( const ZModule<Integer> &lhs,
                            const ZModule<Integer> &rhs) noexcept
{
   return static_cast<typename ZModule<Integer>::value_type>(lhs) <=
            static_cast<typename ZModule<Integer>::value_type>(rhs);
}
template<auto Integer>
constexpr bool operator>  ( const ZModule<Integer> &lhs,
                            const ZModule<Integer> &rhs) noexcept
{
   return static_cast<typename ZModule<Integer>::value_type>(lhs) >
            static_cast<typename ZModule<Integer>::value_type>(rhs);
}
template<auto Integer>
constexpr bool operator>= ( const ZModule<Integer> &lhs,
                            const ZModule<Integer> &rhs) noexcept
{
   return static_cast<typename ZModule<Integer>::value_type>(lhs) >=
            static_cast<typename ZModule<Integer>::value_type>(rhs);
}

// Comparison between elements of different modular rigns.
//
// This will most likely be removed, as there's no good way of defining
// such comparisons (as it depends on which ring we are comparing them),
// but will leave it here for now.
template<auto Integer1, auto Integer2>
constexpr bool operator== ( const ZModule<Integer1> &lhs,
                            const ZModule<Integer2> &rhs) noexcept
{
    if constexpr (Integer1 < Integer2)
        return lhs == ZModule<Integer1>(rhs);
    else
        return ZModule<Integer2>(lhs) == rhs;
}
template<auto Integer1, auto Integer2>
constexpr bool operator!= ( const ZModule<Integer1> &lhs,
                            const ZModule<Integer2> &rhs) noexcept
{
    if constexpr (Integer1 < Integer2)
        return lhs != ZModule<Integer1>(rhs);
    else
        return ZModule<Integer2>(lhs) != rhs;
}
template<auto Integer1, auto Integer2>
constexpr bool operator<  ( const ZModule<Integer1> &lhs,
                            const ZModule<Integer2> &rhs) noexcept
{
    if constexpr (Integer1 < Integer2)
        return lhs < ZModule<Integer1>(rhs);
    else
        return ZModule<Integer2>(lhs) < rhs;
}
template<auto Integer1, auto Integer2>
constexpr bool operator<= ( const ZModule<Integer1> &lhs,
                            const ZModule<Integer2> &rhs) noexcept
{
    if constexpr (Integer1 < Integer2)
        return lhs <= ZModule<Integer1>(rhs);
    else
        return ZModule<Integer2>(lhs) <= rhs;
}
template<auto Integer1, auto Integer2>
constexpr bool operator>  ( const ZModule<Integer1> &lhs,
                            const ZModule<Integer2> &rhs) noexcept
{
    if constexpr (Integer1 < Integer2)
        return lhs > ZModule<Integer1>(rhs);
    else
        return ZModule<Integer2>(lhs) > rhs;
}
template<auto Integer1, auto Integer2>
constexpr bool operator>= ( const ZModule<Integer1> &lhs,
                            const ZModule<Integer2> &rhs) noexcept
{
    if constexpr (Integer1 < Integer2)
        return lhs >= ZModule<Integer1>(rhs);
    else
        return ZModule<Integer2>(lhs) >= rhs;
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

// We can't achive the good mathematical notation for the type, but we
// can provide a similar one (Z<n>)
template<auto Integer>
using Z = ZModule<Integer>;

}   // namespace fgs

// This is a technical type trait to define common types between z-modules
// themselves and other types (in case you need it)
namespace std{
    template <auto UInt1, auto UInt2>
    struct common_type<fgs::ZModule<UInt1>, fgs::ZModule<UInt2>>{
        using type =
            conditional_t<(UInt1 < UInt2),  // if (UInt1 < UInt2)
                fgs::ZModule<UInt2>,
            conditional_t<(UInt1 > UInt2),  // else if (UInt1 > UInt2)
                fgs::ZModule<UInt1>,
            conditional_t<                  // else
                is_same_v<
                    common_type_t<  // We check the common type of the underlined values
                        typename fgs::ZModule<UInt1>::value_type,
                        typename fgs::ZModule<UInt2>::value_type>,
                    typename fgs::ZModule<UInt1>::value_type>,
                fgs::ZModule<UInt1>,
                fgs::ZModule<UInt2>
            >>>;
    };

    template <auto UInt, typename T>
    struct common_type<fgs::ZModule<UInt>, T>{
        using type = common_type_t<typename fgs::ZModule<UInt>::value_type, T>;
    };

    template <auto UInt, typename T>
    struct common_type<T, fgs::ZModule<UInt>>{
        using type = common_type_t<T, typename fgs::ZModule<UInt>::value_type>;
    };
}

// We don't need this macro anymore, so we undefine it
#undef FGS_EXCEPTIONS_ENABLED

#endif
