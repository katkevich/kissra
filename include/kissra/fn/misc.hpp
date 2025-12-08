#pragma once

namespace kissra {
namespace functor {

struct fn_size_t {
    template <typename T>
    static constexpr auto operator()(T&& x) {
        using std::size;
        return size(x);
    }
};

struct fn_ssize_t {
    template <typename T>
    static constexpr auto operator()(T&& x) {
        using std::size;
        using signed_t = std::make_signed_t<decltype(size(x))>;
        return static_cast<signed_t>(size(x));
    }
};
} // namespace functor

namespace fn {
constexpr kissra::functor::fn_size_t size;
constexpr kissra::functor::fn_ssize_t ssize;
} // namespace fn
} // namespace kissra