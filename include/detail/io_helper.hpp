#ifndef _Z_MODULE_UNICODE_FORMAT_HPP_
#define _Z_MODULE_UNICODE_FORMAT_HPP_

#ifdef FGS_EXCEPTIONS_SUPPORT
    #include <exception>
    #include <regex>
#endif
#include <string>

#include "concepts.hpp"

namespace fgs::detail{
    using namespace concepts;

#ifdef FGS_UNICODE_SUPPORT
    constexpr const char* integer_set_symbol = "\u2124";
#else
    constexpr const char* integer_set_symbol = "Z";
#endif
    /* Function that returns any integral number in subscript (subindexs)
     *
     * If FGS_UNICODE_SUPPORT is not enabled, it will return the string "_n"
     */
    template <Integral IntType>
    std::string subindex_string (IntType n){
        std::string subindex;

#ifdef FGS_UNICODE_SUPPORT
        // We need to keep in mind the minus sing if n < 0
        const std::string::size_type pos = (n < 0)
            ? subindex += "\u208B", n=-n, 1
            : 0;

        while (n > 0){
            switch (n%10) {
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
    template <Integral IntType, typename InputIt>
    constexpr IntType mod_aux(InputIt first, InputIt last, IntType N)
        noexcept (!FGS_EXCEPTIONS_ENABLED)
    {
        // If exceptions are enabled, we parse the string and throw if it
        // cannot be converted to an integer
#ifdef FGS_EXCEPTIONS_SUPPORT
        if (!std::regex_match(first, last, std::regex{"^[+-]?[0-9]+$"}))
            throw std::invalid_argument("The string cannot be converted to an integer");
#endif
        // We need to check if the string represents a negative number
        bool is_negative = *first == '-';
        // If the first char is one of '+' or '-', we advance the first iterator
        if (is_negative || *first == '+')
            ++first;

        IntType res = 0;

        for (; first != last; ++first)
            res = (res*10 + static_cast<IntType>(*first - '0')) % N;

        // Little adjustment in case it was negative
        if (is_negative)
            res = N - res;

        return res;
    }
}  // namespace fgs::detail

#endif
