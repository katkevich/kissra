#pragma once
#include "kissra/concepts.hpp"
#include "kissra/misc/utility.hpp"
#include "kissra/type_traits.hpp"
#include <vector>

namespace kissra {
template <typename Tag>
struct collect_mixin {
    template <template <typename...> typename TTo = std::vector, kissra::mut TSelf>
    [[nodiscard]] constexpr auto collect(this TSelf&& self) {
        using val_t = kissra::iter_value_t<TSelf>;
        using ref_t = kissra::iter_reference_t<TSelf>;
        using container_t = TTo<val_t>;

        container_t result;
        if constexpr (kissra::is_sized_v<TSelf> && kissra::can_reserve<container_t>) {
            result.reserve(self.size());
        }

        /**
         * `optional<T>` - should move
         * `optional<T&>` - should copy
         * `optional<T&&>` - should move
         */
        while (auto item = self.next()) {
            if constexpr (kissra::can_push_back<container_t, ref_t>) {
                result.push_back(std::forward_like<ref_t>(*item));
            } else if constexpr (kissra::can_insert<container_t, ref_t>) {
                result.insert(std::ranges::end(result), std::forward_like<ref_t>(*item));
            }
        }

        return result;
    }
};
} // namespace kissra