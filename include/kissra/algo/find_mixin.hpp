#pragma once
#include "kissra/concepts.hpp"
#include "kissra/type_traits.hpp"
#include <concepts>
#include <functional>

namespace kissra {
template <typename Tag>
struct find_mixin {
    template <kissra::mut TSelf, typename TValue>
    auto find(this TSelf&& self, const TValue& value) {
        while (auto item = self.next()) {
            if (eq(*item, value)) {
                return item;
            }
        }
        return iter_result_t<TSelf>{};
    }

    template <kissra::mut TSelf, typename TFn>
        requires std::regular_invocable<TFn, iter_reference_t<TSelf>>
    auto find_if(this TSelf&& self, TFn fn) {
        while (auto item = self.next()) {
            if (std::invoke(fn, *item)) {
                return item;
            }
        }
        return iter_result_t<TSelf>{};
    }

    template <kissra::mut TSelf, typename TValue, typename TProj>
        requires std::regular_invocable<TProj, iter_reference_t<TSelf>>
    auto find(this TSelf&& self, const TValue& value, TProj proj) {
        while (auto item = self.next()) {
            if (eq(std::invoke(proj, *item), value)) {
                return item;
            }
        }
        return iter_result_t<TSelf>{};
    }

    template <kissra::mut TSelf, typename TValue>
    auto find_not(this TSelf&& self, TValue&& value) {
        while (auto item = self.next()) {
            if (!eq(*item, value)) {
                return item;
            }
        }
        return iter_result_t<TSelf>{};
    }

    template <kissra::mut TSelf, typename TFn>
        requires std::regular_invocable<TFn, iter_reference_t<TSelf>>
    auto find_if_not(this TSelf&& self, TFn fn) {
        while (auto item = self.next()) {
            if (!std::invoke(fn, *item)) {
                return item;
            }
        }
        return iter_result_t<TSelf>{};
    }

    template <kissra::mut TSelf, typename TValue, typename TProj>
        requires std::regular_invocable<TProj, iter_reference_t<TSelf>>
    auto find_not(this TSelf&& self, TValue&& value, TProj proj) {
        while (auto item = self.next()) {
            if (!eq(std::invoke(proj, *item), value)) {
                return item;
            }
        }
        return iter_result_t<TSelf>{};
    }

    template <kissra::mut TSelf, typename TValue>
    bool contains(this TSelf&& self, const TValue& value) {
        return self.find(value).has_value();
    }

    template <kissra::mut TSelf, typename TFn>
        requires std::regular_invocable<TFn, iter_reference_t<TSelf>>
    bool contains_if(this TSelf&& self, TFn fn) {
        return self.find_if(fn).has_value();
    }

    template <kissra::mut TSelf, typename TValue, typename TProj>
        requires std::regular_invocable<TProj, iter_reference_t<TSelf>>
    bool contains(this TSelf&& self, const TValue& value, TProj proj) {
        return self.find(value, proj).has_value();
    }

    template <kissra::mut TSelf, typename TValue>
    bool contains_not(this TSelf&& self, const TValue& value) {
        return self.find_not(value).has_value();
    }

    template <kissra::mut TSelf, typename TFn>
        requires std::regular_invocable<TFn, iter_reference_t<TSelf>>
    bool contains_if_not(this TSelf&& self, TFn fn) {
        return self.find_if_not(fn).has_value();
    }

    template <kissra::mut TSelf, typename TValue, typename TProj>
        requires std::regular_invocable<TProj, iter_reference_t<TSelf>>
    bool contains_not(this TSelf&& self, const TValue& value, TProj proj) {
        return self.find_not(value, proj).has_value();
    }

private:
    template <typename Lhs, typename Rhs>
    static bool eq(const Lhs& l, const Rhs& r) {
        if constexpr (requires { l == r; }) {
            return l == r;
        } else {
            return !(l != r);
        }
    }
};
} // namespace kissra