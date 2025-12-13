#pragma once
#include "kissra/algo/size_mixin.hpp"
#include "kissra/iter/iter_base.hpp"
#include <algorithm>
#include <type_traits>

namespace kissra {
template <typename TBaseIter, typename TFn, typename TMixins>
    requires std::regular_invocable<TFn, typename TBaseIter::reference>
class transform_iter : public iter_base<TBaseIter>, public size_mixin, public TMixins {
    friend class size_mixin;

public:
    using value_type = std::invoke_result_t<TFn, typename TBaseIter::reference>;
    using reference = value_type;
    using result_t = kissra::optional<reference>;

    static constexpr bool is_sized = TBaseIter::is_sized;
    static constexpr bool is_common = TBaseIter::is_common;
    static constexpr bool is_forward = TBaseIter::is_forward;
    static constexpr bool is_bidir = TBaseIter::is_bidir;
    static constexpr bool is_random = TBaseIter::is_random;

    template <typename UBaseIter>
    transform_iter(UBaseIter&& base_iter, TFn fn)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , fn(fn) {}

    [[nodiscard]] result_t next() {
        if (auto item = this->base_iter.next()) {
            return std::invoke(this->fn, *item);
        }
        return {};
    }

    [[nodiscard]] result_t next_back()
        requires is_common && is_bidir
    {
        if (auto item = this->base_iter.next_back()) {
            return std::invoke(this->fn, *item);
        }
        return {};
    }

    [[nodiscard]] result_t nth(std::size_t n) {
        if (auto item = this->base_iter.nth(n)) {
            return std::invoke(this->fn, *item);
        }
        return {};
    }

    [[nodiscard]] result_t nth_back(std::size_t n)
        requires is_common && is_bidir
    {
        if (auto item = this->base_iter.nth_back(n)) {
            return std::invoke(this->fn, *item);
        }
        return {};
    }

    std::size_t advance(std::size_t n) {
        return this->base_iter.advance(n);
    }

    std::size_t advance_back(std::size_t n)
        requires is_bidir
    {
        return this->base_iter.advance_back(n);
    }

private:
    TFn fn;
};

struct transform_mixin {
    template <typename TSelf, typename TFn, typename DeferInstantiation = void>
    auto transform(this TSelf&& self, TFn fn) {
        auto mixins = registered_mixins<DeferInstantiation>();
        return transform_iter<std::remove_cvref_t<TSelf>, TFn, decltype(mixins)>{ std::forward<TSelf>(self), fn };
    }
};
} // namespace kissra