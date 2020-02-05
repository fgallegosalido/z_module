#ifndef Z_MODULE_IO_HELPER_HPP__
#define Z_MODULE_IO_HELPER_HPP__

#include "concepts.hpp"

#include <string>

#ifdef FGS_EXCEPTIONS_SUPPORT
    #include <exception>
    #include <regex>
#endif

namespace fgs::detail{
#ifdef FGS_UNICODE_SUPPORT
    constexpr const char* integer_set_symbol = "\u2124";
#else
    constexpr const char* integer_set_symbol = "Z";
#endif
    /* Function that returns any integral number in subscript (subindexs)
     *
     * If FGS_UNICODE_SUPPORT is not enabled, it will return the string "_n"
     */
    std::string subindex_string (std::integral auto n){
        std::string subindex;

#ifdef FGS_UNICODE_SUPPORT
        // We need to keep in mind the minus sing if n < 0
        const std::string::size_type pos = (n < 0)
            ? subindex += "\u208B", n=-n, 1
            : 0;

        while (n > 0){
            switch (n%10) { // This switch keeps clang-tidy happy
                case 0: subindex.insert(pos, "\u2080"); break;
                case 1: subindex.insert(pos, "\u2081"); break;
                case 2: subindex.insert(pos, "\u2082"); break;
                case 3: subindex.insert(pos, "\u2083"); break;
                case 4: subindex.insert(pos, "\u2084"); break;
                case 5: subindex.insert(pos, "\u2085"); break;
                case 6: subindex.insert(pos, "\u2086"); break;
                case 7: subindex.insert(pos, "\u2087"); break;
                case 8: subindex.insert(pos, "\u2088"); break;
                case 9: subindex.insert(pos, "\u2089"); break;
            }
            n /= 10;
        }
#else
        subindex = "_" + std::to_string(n);
#endif

        return subindex;
    }

    // Function to calculate the module of an integer represented as a string
    template <typename CharT, typename Traits>
    constexpr auto mod_aux(const std::basic_string_view<CharT, Traits> &s, std::integral auto N)
#   ifndef FGS_EXCEPTIONS_SUPPORT
        noexcept
#   endif
    {
        // If exceptions are enabled, we parse the string and throw if it
        // cannot be converted to an integer
        //
        // Otherwise, undefined behaviour
#ifdef FGS_EXCEPTIONS_SUPPORT
        if (!std::regex_match(begin(s), end(s), std::regex{"^[+-]?[0-9]+$"}))
            throw std::invalid_argument("The string cannot be converted to an integer");
#endif
        decltype(N) res = 0;

        for (std::size_t i=(s[0]=='+' || s[0]=='-')?1:0; i<s.size(); ++i)
            res = (res*10 + static_cast<decltype(N)>(s[i] - '0')) % N;

        // Little adjustment at return point in case it was negative
        return (s[0] == '-') ? N - res : res;
    }
}  // namespace fgs::detail

#endif
