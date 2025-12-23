#pragma once
#include "doctest/doctest.h"
#include <format>
#include <forward_list>
#include <list>
#include <vector>

namespace doctest {
template <typename T>
struct StringMaker<std::vector<T>> {
    static String convert(const std::vector<T>& v) {
        return std::format("{}", v).c_str();
    }
};
} // namespace doctest