#pragma once
#include "kissra/iter/all_iter.hpp"
#include "kissra/iter/iter_base.hpp"
#include "kissra/misc/into_iter.hpp"
#include "kissra/misc/type_list.hpp"
#include "kissra/misc/utility.hpp"
#include <algorithm>
#include <type_traits>

namespace kissra {
template <typename TBaseIter, typename TItersTypeList, typename TMixins>
class zip_iter;

template <typename TBaseIter, typename... TIters, typename TMixins>
class zip_iter<TBaseIter, tmp::type_list<TIters...>, TMixins> : public iter_base<TBaseIter>, public TMixins {
public:
    using value_type = std::tuple<typename TBaseIter::reference, typename TIters::reference...>;
    using reference = value_type;
    using const_reference = value_type;

    template <typename TSelf>
    using ref_t = reference;

    template <typename TSelf>
    using result_t = kissra::optional<ref_t<TSelf>>;

    static constexpr bool is_sized = TBaseIter::is_sized && (TIters::is_sized && ...);
    static constexpr bool is_common = TBaseIter::is_common && (TIters::is_common && ...);
    static constexpr bool is_forward = TBaseIter::is_forward && (TIters::is_forward && ...);
    static constexpr bool is_bidir = TBaseIter::is_bidir && (TIters::is_bidir && ...);
    static constexpr bool is_random = TBaseIter::is_random && (TIters::is_random && ...);

    template <typename UBaseIter, typename... UIters>
    zip_iter(UBaseIter&& base_iter, UIters&&... iters)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , iters(std::move(iters)...) {}

    template <typename TSelf>
    [[nodiscard]] result_t<TSelf> next(this TSelf&& self) {
        auto& [... iters_pack] = self.iters;
        auto [... nexts_pack] = std::tuple{ self.base_iter.next(), iters_pack.next()... };

        if ((nexts_pack.has_value() && ...)) {
            return ref_t<TSelf>{ KISSRA_FWD(*nexts_pack)... };
        }

        return {};
    }

    template <typename TSelf>
        requires is_sized && is_bidir && is_common && ((is_sized_v<TIters> && is_bidir_v<TIters> && is_common_v<TIters>) && ...)
    [[nodiscard]] result_t<TSelf> next_back(this TSelf&& self) {
        /* align the tails */
        self.advance_back(0);

        auto& [... iters_pack] = self.iters;
        auto [... next_backs_pack] = std::tuple{ self.base_iter.next_back(), iters_pack.next_back()... };

        if ((next_backs_pack.has_value() && ...)) {
            return ref_t<TSelf>{ KISSRA_FWD(*next_backs_pack)... };
        }

        return {};
    }

    template <typename TSelf>
    [[nodiscard]] result_t<TSelf> nth(this TSelf&& self, std::size_t n) {
        auto& [... iters_pack] = self.iters;
        auto [... nths_pack] = std::tuple{ self.base_iter.nth(n), iters_pack.nth(n)... };

        if ((nths_pack.has_value() && ...)) {
            return ref_t<TSelf>{ KISSRA_FWD(*nths_pack)... };
        }

        return {};
    }

    template <typename TSelf>
        requires is_sized && is_bidir && is_common && ((is_sized_v<TIters> && is_bidir_v<TIters> && is_common_v<TIters>) && ...)
    [[nodiscard]] result_t<TSelf> nth_back(this TSelf&& self, std::size_t n) {
        /* align the tails */
        self.advance_back(0);

        auto& [... iters_pack] = self.iters;
        auto [... next_backs_pack] = std::tuple{ self.base_iter.nth_back(n), iters_pack.nth_back(n)... };

        if ((next_backs_pack.has_value() && ...)) {
            return ref_t<TSelf>{ KISSRA_FWD(*next_backs_pack)... };
        }

        return {};
    }

    template <typename TSelf>
    std::size_t advance(this TSelf&& self, std::size_t n) {
        auto& [... iters_pack] = self.iters;
        std::array advancements{ self.base_iter.advance(n), iters_pack.advance(n)... };

        return *std::ranges::min_element(advancements);
    }

    template <typename TSelf>
        requires is_sized && is_bidir && ((is_sized_v<TIters> && is_bidir_v<TIters>) && ...)
    std::size_t advance_back(this TSelf&& self, std::size_t n) {
        auto& [... iters_pack] = self.iters;

        const auto base_iter_size = self.base_iter.size();
        const auto [... iter_sizes] = std::array{ iters_pack.size()... };

        const auto sizes = std::array{ base_iter_size, iter_sizes... };
        const auto min_size = *std::ranges::min_element(sizes);

        const auto base_iter_tail = base_iter_size - min_size;
        const auto [... iter_tails] = std::array{ (iter_sizes - min_size)... };

        const auto advancement = self.base_iter.advance_back(base_iter_tail + n);
        (iters_pack.advance_back(iter_tails + n), ...);

        return advancement - base_iter_tail;
    }

    template <typename TSelf>
        requires is_sized
    auto size(this TSelf&& self) {
        auto& [... iters_pack] = self.iters;
        const auto sizes = std::array{ self.base_iter.size(), iters_pack.size()... };

        return *std::ranges::min_element(sizes);
    }

private:
    std::tuple<TIters...> iters;
};

struct zip_mixin {
    template <typename TSelf, typename... TContainersOrKissraIters, typename DeferInstantiation = void>
        requires(((std::ranges::range<TContainersOrKissraIters> && std::is_lvalue_reference_v<TContainersOrKissraIters>) ||
                     kissra::iterator<TContainersOrKissraIters>) &&
            ...)
    auto zip(this TSelf&& self, TContainersOrKissraIters&&... containers_or_kissra_iters) {
        using mixins_t = decltype(registered_mixins<DeferInstantiation>());

        using iters_type_list =
            tmp::type_list<std::remove_reference_t<decltype(into_kissra_iter<mixins_t>(KISSRA_FWD(containers_or_kissra_iters)))>...>;

        return zip_iter<std::remove_cvref_t<TSelf>, iters_type_list, mixins_t>{ KISSRA_FWD(self),
            into_kissra_iter<mixins_t>(KISSRA_FWD(containers_or_kissra_iters))... };
    }
};
} // namespace kissra