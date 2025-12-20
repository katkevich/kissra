#pragma once
#include "kissra/concepts.hpp"
#include "kissra/misc/utility.hpp"
#include <cstddef>

namespace kissra {
namespace functor {
template <std::size_t MemberIdx>
struct member_t {
    template <typename T>
        requires std::is_aggregate_v<std::remove_cvref_t<T>> || tuple_like<T>
    static constexpr decltype(auto) operator()(T&& x) {
        auto&& [... xs] = x;
        using member_t = decltype(xs...[MemberIdx]);

        /**
         * T&.member& -> return member&
         * T&&.member& -> return member&
         * T&.member&& -> return member&&
         * T&&.member&& -> return member&&
         *
         * T&.member -> return member&
         * T&&.member -> return member
         */
        if constexpr (std::is_reference_v<member_t>) {
            /* if `member` is a reference return it as a reference (as-is, preserving the value category). */
            return std::forward<member_t>(xs...[MemberIdx]);
        } else {
            if constexpr (std::is_reference_v<T>) {
                /* `return (<expr>)` is being deduced as a reference */
                return (xs...[MemberIdx]);
            } else {
                /* Automatic move won't work. */
                return member_t(std::move(xs...[MemberIdx]));
            }
        }
    }
};
} // namespace functor

namespace fn {
template <std::size_t MemberIdx>
constexpr kissra::functor::member_t<MemberIdx> member;
}
} // namespace kissra