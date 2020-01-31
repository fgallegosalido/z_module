#ifndef Z_MODULE_COMMON_TYPE_HPP__
#define Z_MODULE_COMMON_TYPE_HPP__

#include <concepts>
#include <type_traits>

namespace fgs{
    // Forward declare the class
    template <std::integral auto Integer> requires (Integer > 1)
    class ZModule;
}

// This is a technical type trait to define common types between z-modules
// themselves and other types (in case you need it)
namespace std{
    // If two ZModule's have the same cardinality, their common type is
    // the one which has bigger underlined unsigned integer type
    template <auto UInt1, auto UInt2>
    requires (UInt1 == UInt2)
    struct common_type<fgs::ZModule<UInt1>, fgs::ZModule<UInt2>>{
        using type = conditional_t<sizeof(typename fgs::ZModule<UInt1>::value_type) <
                                   sizeof(typename fgs::ZModule<UInt2>::value_type),
            fgs::ZModule<UInt2>,
            fgs::ZModule<UInt1>
        >;
    };

    // The common type between two ZModule's of different cardinality
    // cannot be a ZModule, so we just take the common type of the
    // underlined types
    template <auto UInt1, auto UInt2>
    requires (UInt1 != UInt2)
    struct common_type<fgs::ZModule<UInt1>, fgs::ZModule<UInt2>>{
        using type = common_type_t<  // We check the common type of the underlined values
            typename fgs::ZModule<UInt1>::value_type,
            typename fgs::ZModule<UInt2>::value_type
        >;
    };

    // For any other type, we use the underlined type of the ZModule
    template <auto UInt, typename T>
    struct common_type<fgs::ZModule<UInt>, T>{
        using type = common_type_t<typename fgs::ZModule<UInt>::value_type, T>;
    };

    // For any other type, we use the underlined type of the ZModule
    template <auto UInt, typename T>
    struct common_type<T, fgs::ZModule<UInt>>{
        using type = common_type_t<T, typename fgs::ZModule<UInt>::value_type>;
    };
}

#endif
