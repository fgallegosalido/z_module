#ifndef _Z_MODULE_CONCEPTS_HPP_
#define _Z_MODULE_CONCEPTS_HPP_

#include <type_traits>

#ifdef __GNUG__
    #define FGS_CONCEPT_DEFINITION concept bool
#else
    #define FGS_CONCEPT_DEFINITION concept
#endif

namespace fgs::concepts{
    template <typename T>
    FGS_CONCEPT_DEFINITION Integral = std::is_integral_v<T>;

    template <typename T>
    FGS_CONCEPT_DEFINITION SignedIntegral = Integral<T> && std::is_signed_v<T>;

    template <typename T>
    FGS_CONCEPT_DEFINITION UnsignedIntegral = Integral<T> && !SignedIntegral<T>;

    template <typename T>
    FGS_CONCEPT_DEFINITION Destructible = std::is_nothrow_destructible_v<T>;

    template <typename T, typename... Args>
    FGS_CONCEPT_DEFINITION Constructible = Destructible<T> && std::is_constructible_v<T, Args...>;
}

#undef FGS_CONCEPT_DEFINITION
#endif
