#pragma once

namespace kissra {
namespace functor {
/* x % y == 0 */
template <typename Rhs>
struct divisible_by_val_t {
    template <typename Lhs>
    constexpr bool operator()(Lhs l) const {
        return l % r == 0;
    }

    Rhs r;
};

struct divisible_by_t {
    template <typename Lhs, typename Rhs>
    static constexpr bool operator()(Lhs l, Rhs r) {
        return l % r == 0;
    }

    template <typename Rhs>
    static constexpr auto operator()(Rhs r) {
        return divisible_by_val_t{ r };
    }
};

template <auto Rhs>
struct divisible_by_c_t {
    template <typename Lhs>
    static constexpr bool operator()(Lhs l) {
        return l % Rhs == 0;
    }
};

struct even_t {
    template <typename Lhs>
    static constexpr bool operator()(Lhs l) {
        return l % 2 == 0;
    }
};

struct odd_t {
    template <typename Lhs>
    static constexpr bool operator()(Lhs l) {
        return l % 2 == 1;
    }
};
} // namespace functor

namespace fn {
template <auto Rhs>
constexpr kissra::functor::divisible_by_c_t<Rhs> divisible_by_c;
constexpr kissra::functor::divisible_by_t divisible_by;

constexpr kissra::functor::even_t even;
constexpr kissra::functor::odd_t odd;
} // namespace fn
} // namespace kissra