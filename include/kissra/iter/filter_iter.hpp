#pragma once
#include "kissra/concepts.hpp"
#include "kissra/iter/iter_base.hpp"
#include "kissra/misc/functional.hpp"
#include <functional>
#include <utility>

namespace kissra {
template <typename TBaseIter, typename TFn, typename TMixins>
    requires kissra::regular_invocable<TFn, typename TBaseIter::reference>
class filter_iter : public iter_base<TBaseIter>, public TMixins {
public:
    using value_type = typename TBaseIter::value_type;
    using reference = typename TBaseIter::reference;
    using result_t = typename TBaseIter::result_t;

    static constexpr bool is_sized = false;
    static constexpr bool is_common = TBaseIter::is_common;
    static constexpr bool is_forward = TBaseIter::is_forward;
    static constexpr bool is_bidir = TBaseIter::is_bidir;
    static constexpr bool is_random = false;

    template <typename UBaseIter>
    filter_iter(UBaseIter&& base_iter, TFn fn)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , fn(fn) {}

    [[nodiscard]] result_t next() {
        while (auto item = this->base_iter.next()) {
            if (kissra::invoke(this->fn, std::forward_like<reference>(*item))) {
                return item;
            }
        }
        return {};
    }

    [[nodiscard]] result_t next_back()
        requires is_common && is_bidir
    {
        while (auto item = this->base_iter.next_back()) {
            if (kissra::invoke(this->fn, std::forward_like<reference>(*item))) {
                return item;
            }
        }
        return {};
    }

    [[nodiscard]] result_t nth(std::size_t n) {
        for (auto item = this->base_iter.front(); item; item = this->base_iter.nth(1)) {
            if (kissra::invoke(this->fn, std::forward_like<reference>(*item))) {
                if (n-- == 0) {
                    return item;
                }
            }
        }
        return {};
    }

    [[nodiscard]] result_t nth_back(std::size_t n)
        requires is_common && is_bidir
    {
        for (auto item = this->base_iter.back(); item; item = this->base_iter.nth_back(1)) {
            if (kissra::invoke(this->fn, std::forward_like<reference>(*item))) {
                if (n-- == 0) {
                    return item;
                }
            }
        }
        return {};
    }

    std::size_t advance(std::size_t n) {
        std::size_t offset = 0;
        for (auto item = this->base_iter.front(); item; item = this->base_iter.nth(1)) {
            if (kissra::invoke(this->fn, std::forward_like<reference>(*item))) {
                if (offset++ == n) {
                    break;
                }
            }
        }
        return offset;
    }

    std::size_t advance_back(std::size_t n)
        requires is_common && is_bidir
    {
        std::size_t offset = 0;
        for (auto item = this->base_iter.back(); item; item = this->base_iter.nth_back(1)) {
            if (kissra::invoke(this->fn, std::forward_like<reference>(*item))) {
                if (offset++ == n) {
                    break;
                }
            }
        }
        return offset;
    }

private:
    TFn fn;
};

struct filter_mixin {
    template <typename TSelf, typename TFn, typename DeferInstantiation = void>
    auto filter(this TSelf&& self, TFn fn) {
        auto mixins = registered_mixins<DeferInstantiation>();
        return filter_iter<std::remove_cvref_t<TSelf>, TFn, decltype(mixins)>{ std::forward<TSelf>(self), fn };
    }
};
} // namespace kissra