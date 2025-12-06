#pragma once
#include "kissra/misc/fwd.hpp"
#include "kissra/misc/static_string.hpp"
#include <cstddef>
#include <format>
#include <string>

namespace kissra {
namespace functor {
template <kissra::static_string FmtString>
struct fmt_c_t {
    template <typename T>
    static constexpr std::string operator()(T&& x) {
        return std::format(std::string_view{ FmtString.str.data(), FmtString.str.size() - 1 }, std::forward<T>(x));
    }
};

struct fmt_t {
    template <typename T>
    constexpr std::string operator()(T&& x) const {
        return std::vformat(fmt, std::make_format_args(KISSRA_FWD(x)));
    }

    std::string fmt;
};

struct fmt_builder_t {
    constexpr auto operator()(const std::string& fmt) const {
        return fmt_t{ fmt };
    }

    constexpr auto operator()(std::string&& fmt) const {
        return fmt_t{ std::move(fmt) };
    }
};
} // namespace functor

namespace fn {
template <kissra::static_string FmtString>
constexpr kissra::functor::fmt_c_t<FmtString> format_c;

constexpr kissra::functor::fmt_builder_t format;
} // namespace fn
} // namespace kissra