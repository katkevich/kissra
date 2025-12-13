#pragma once
#include "kissra/iter/iter_base.hpp"
#include <functional>

namespace kissra {
template <typename TBaseIter, typename TMixins>
class drop_last_iter : public iter_base<TBaseIter>, public TMixins {
public:
    using value_type = typename TBaseIter::value_type;
    using reference = typename TBaseIter::reference;
    using result_t = typename TBaseIter::result_t;

    static constexpr bool is_sized = TBaseIter::is_sized;
    static constexpr bool is_common = TBaseIter::is_common;
    static constexpr bool is_forward = TBaseIter::is_forward;
    static constexpr bool is_bidir = TBaseIter::is_bidir;
    static constexpr bool is_random = TBaseIter::is_random;

    template <typename UBaseIter>
    drop_last_iter(UBaseIter&& base_iter, std::size_t n)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , curr_n(n)
        , n(n) {}

    [[nodiscard]] result_t next()
        requires is_bidir
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
        requires is_bidir
    {
        this->ff();
        return this->base_iter.nth(n);
    }

    [[nodiscard]] result_t nth_back(std::size_t n)
        requires is_common && is_bidir
    {
        const auto total = this->curr_n + n;
        this->curr_n = 0;
        return this->base_iter.nth_back(total);
    }

    std::size_t advance(std::size_t n)
        requires is_bidir
    {
        this->ff();
        return this->base_iter.advance(n);
    }

    std::size_t advance_back(std::size_t n)
        requires is_bidir
    {
        const auto total = this->curr_n + n;
        this->curr_n = 0;
        return this->base_iter.advance_back(total);
    }

    auto size() const
        requires is_sized
    {
        const std::size_t base_size = this->base_iter.size();
        return base_size - std::min(base_size, this->curr_n);
    }

private:
    void ff() {
        if (this->curr_n) {
            this->advance_back(0);
        }
    }

private:
    std::size_t curr_n;
    std::size_t n;
};

struct drop_last_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    auto drop_last(this TSelf&& self, std::size_t n) {
        auto mixins = registered_mixins<DeferInstantiation>();
        return drop_last_iter<std::remove_cvref_t<TSelf>, decltype(mixins)>{ std::forward<TSelf>(self), n };
    }
};
} // namespace kissra