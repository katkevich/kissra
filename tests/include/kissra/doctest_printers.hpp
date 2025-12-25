#pragma once
#include "doctest/doctest.h"
#include <format>
#include <forward_list>
#include <list>
#include <tuple>
#include <vector>

namespace doctest {
template <typename T>
struct StringMaker<std::vector<T>> {
    static String convert(const std::vector<T>& v) {
        return std::format("{}", v).c_str();
    }
};

template <typename... Ts>
struct StringMaker<std::tuple<Ts...>> {
    static String convert(const std::tuple<Ts...>& t) {
        return std::format("{}", t).c_str();
    }
};
} // namespace doctest