#pragma once
#include <cstddef>

namespace kissra {
template <std::size_t N>
struct static_string {
    constexpr static_string(const char (&str)[N]) {
        for (int i = 0; i < N; ++i) {
            this->str[i] = str[i];
        }
    }

    std::array<char, N> str = {};
};

template <std::size_t N>
static_string(const char (&)[N]) -> static_string<N>;
} // namespace kissra