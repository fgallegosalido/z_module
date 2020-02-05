#ifndef Z_MODULE_CONCEPTS__
#define Z_MODULE_CONCEPTS__

#include <version>

#ifdef __cpp_concepts   // Check if the compiler supports concepts
#   ifdef __cpp_lib_concepts    // We have the concept header, so we include it
#include <concepts>
#   else                        // We implement the needed concepts for the library

#include <type_traits>
#include <utility>

namespace std{
    namespace detail{
        template <typename T, typename U>
        concept SameHelper = is_same_v<T, U>;
    }

    template <typename T, typename U>
    concept same_as = detail::SameHelper<T, U> && detail::SameHelper<U, T>;

    template <typename T>
    concept integral = is_integral_v<T>;

    template <typename T>
    concept signed_integral = integral<T> && is_signed_v<T>;

    template <typename T>
    concept unsigned_integral = integral<T> && !signed_integral<T>;

    template <typename T>
    concept destructible = is_nothrow_destructible_v<T>;

    template <typename T, typename ...Args>
    concept constructible_from = destructible<T> && is_constructible_v<T, Args...>;
} // namespace std

#   endif
#else
    #error "The compiler must support concepts"
#endif

#endif
