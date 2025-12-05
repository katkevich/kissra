#pragma once

namespace kissra {
namespace fn {
/* operator < */
template <typename Rhs>
struct lt_val_t {
    template <typename Lhs>
    constexpr bool operator()(Lhs&& l) const {
        return l < r;
    }

    Rhs r;
};

struct lt_t {
    template <typename Lhs, typename Rhs>
    static constexpr bool operator()(Lhs&& l, Rhs&& r) {
        return l < r;
    }

    template <typename Rhs>
    static constexpr auto operator()(Rhs&& r) {
        return lt_val_t{ std::forward<Rhs>(r) };
    }
};

template <auto Rhs>
struct lt_c_t {
    template <typename Lhs>
    static constexpr bool operator()(Lhs&& l) {
        return l < Rhs;
    }
};


/* operator <= */
template <typename Rhs>
struct le_val_t {
    template <typename Lhs>
    constexpr bool operator()(Lhs&& l) const {
        return l <= r;
    }

    Rhs r;
};

struct le_t {
    template <typename Lhs, typename Rhs>
    static constexpr bool operator()(Lhs&& l, Rhs&& r) {
        return l <= r;
    }

    template <typename Rhs>
    static constexpr auto operator()(Rhs&& r) {
        return le_val_t{ std::forward<Rhs>(r) };
    }
};

template <auto Rhs>
struct le_c_t {
    template <typename Lhs>
    static constexpr bool operator()(Lhs&& l) {
        return l <= Rhs;
    }
};


/* operator > */
template <typename Rhs>
struct gt_val_t {
    template <typename Lhs>
    constexpr bool operator()(Lhs&& l) const {
        return l > r;
    }

    Rhs r;
};

struct gt_t {
    template <typename Lhs, typename Rhs>
    static constexpr bool operator()(Lhs&& l, Rhs&& r) {
        return l > r;
    }

    template <typename Rhs>
    static constexpr auto operator()(Rhs&& r) {
        return gt_val_t{ std::forward<Rhs>(r) };
    }
};

template <auto Rhs>
struct gt_c_t {
    template <typename Lhs>
    static constexpr bool operator()(Lhs&& l) {
        return l > Rhs;
    }
};


/* operator >= */
template <typename Rhs>
struct ge_val_t {
    template <typename Lhs>
    constexpr bool operator()(Lhs&& l) const {
        return l >= r;
    }

    Rhs r;
};

struct ge_t {
    template <typename Lhs, typename Rhs>
    static constexpr bool operator()(Lhs&& l, Rhs&& r) {
        return l >= r;
    }

    template <typename Rhs>
    static constexpr auto operator()(Rhs&& r) {
        return ge_val_t{ std::forward<Rhs>(r) };
    }
};

template <auto Rhs>
struct ge_c_t {
    template <typename Lhs>
    static constexpr bool operator()(Lhs&& l) {
        return l >= Rhs;
    }
};

/* operator == */
template <typename Rhs>
struct eq_val_t {
    template <typename Lhs>
    constexpr bool operator()(Lhs&& l) const {
        return l == r;
    }

    Rhs r;
};

struct eq_t {
    template <typename Lhs, typename Rhs>
    static constexpr bool operator()(Lhs&& l, Rhs&& r) {
        return l == r;
    }

    template <typename Rhs>
    static constexpr auto operator()(Rhs&& r) {
        return eq_val_t{ std::forward<Rhs>(r) };
    }
};

template <auto Rhs>
struct eq_c_t {
    template <typename Lhs>
    static constexpr bool operator()(Lhs&& l) {
        return l == Rhs;
    }
};

/* operator != */
template <typename Rhs>
struct ne_val_t {
    template <typename Lhs>
    constexpr bool operator()(Lhs&& l) const {
        return l != r;
    }

    Rhs r;
};

struct ne_t {
    template <typename Lhs, typename Rhs>
    static constexpr bool operator()(Lhs&& l, Rhs&& r) {
        return l != r;
    }

    template <typename Rhs>
    static constexpr auto operator()(Rhs&& r) {
        return ne_val_t{ std::forward<Rhs>(r) };
    }
};

template <auto Rhs>
struct ne_c_t {
    template <typename Lhs>
    static constexpr bool operator()(Lhs&& l) {
        return l != Rhs;
    }
};
} // namespace fn

template <auto Rhs>
constexpr fn::lt_c_t<Rhs> lt_c;
constexpr fn::lt_t lt;

template <auto Rhs>
constexpr fn::le_c_t<Rhs> le_c;
constexpr fn::le_t le;

template <auto Rhs>
constexpr fn::gt_c_t<Rhs> gt_c;
constexpr fn::gt_t gt;

template <auto Rhs>
constexpr fn::ge_c_t<Rhs> ge_c;
constexpr fn::ge_t ge;

template <auto Rhs>
constexpr fn::eq_c_t<Rhs> eq_c;
constexpr fn::eq_t eq;

template <auto Rhs>
constexpr fn::ne_c_t<Rhs> ne_c;
constexpr fn::ne_t ne;
} // namespace kissra