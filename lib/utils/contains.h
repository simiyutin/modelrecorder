#pragma once

#include <algorithm>

namespace {
    template <typename T, typename V>
    struct hasFind {
        using yes = char;
        using no = struct {yes dummy[2];};
        template <typename U>
        static auto test(U * u, V * v) -> decltype(u->find(*v), yes());
        static no test(...);

        static const bool value = sizeof(test((T*)0, (V*)0)) == sizeof(yes);
    };

    template<typename CONT, typename VAL>
    using HasNotFind = typename std::enable_if<!hasFind<CONT, VAL>::value, CONT>::type;

    template<typename CONT, typename VAL>
    using HasFind = typename std::enable_if<hasFind<CONT, VAL>::value, CONT>::type;
}

template<typename CONT, typename VAL>
inline bool contains(const CONT & cont, const VAL & value, HasFind<CONT, VAL>* = nullptr) {
    return cont.find(value) != cont.end();
}

template<typename CONT, typename VAL>
inline bool contains(const CONT & cont, const VAL & value, HasNotFind<CONT, VAL>* = nullptr) {
    return std::find(cont.begin(), cont.end(), value) != cont.end();
}
