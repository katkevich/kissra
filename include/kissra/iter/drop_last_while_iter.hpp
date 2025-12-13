#pragma once
#include "kissra/iter/iter_base.hpp"
#include <functional>

namespace kissra {
template <typename TBaseIter, typename TFn, typename TMixins>
    requires std::regular_invocable<TFn, typename TBaseIter::reference>
class drop_last_while_iter : public iter_base<TBaseIter>, public TMixins {
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
    drop_last_while_iter(UBaseIter&& base_iter, TFn fn)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , fn(fn) {}

    [[nodiscard]] result_t next()
        requires is_common && is_bidir
    {
        this->ff();
        return this->base_iter.next();
    }

    [[nodiscard]] result_t next_back()
        requires is_common && is_bidir
    {
        this->ff();
        return this->base_iter.next_back();
    }

    [[nodiscard]] result_t nth(std::size_t n)
        requires is_common && is_bidir
    {
        this->ff();
        return this->base_iter.nth(n);
    }

    [[nodiscard]] result_t nth_back(std::size_t n)
        requires is_common && is_bidir
    {
        this->ff_self();
        return this->base_iter.nth_back(n);
    }

    std::size_t advance(std::size_t n)
        requires is_common && is_bidir
    {
        this->ff();
        return this->base_iter.advance(n);
    }

    std::size_t advance_back(std::size_t n)
        requires is_common && is_bidir
    {
        this->ff_self();
        return this->base_iter.advance_back(n);
    }

private:
    void ff() {
        if (!this->dropped) {
            this->advance_back(0);
        }
    }

    void ff_self() {
        if (!std::exchange(this->dropped, true)) {
            for (auto item = this->base_iter.back(); item; item = this->base_iter.nth_back(1)) {
                if (!std::invoke(this->fn, *item)) {
                    break;
                }
            }
        }
    }

private:
    TFn fn;
    bool dropped{};
};

struct drop_last_while_mixin {
    template <typename TSelf, typename TFn, typename DeferInstantiation = void>
    auto drop_last_while(this TSelf&& self, TFn fn) {
        auto mixins = registered_mixins<DeferInstantiation>();
        return drop_last_while_iter<std::remove_cvref_t<TSelf>, TFn, decltype(mixins)>{ std::forward<TSelf>(self), fn };
    }
};
} // namespace kissra