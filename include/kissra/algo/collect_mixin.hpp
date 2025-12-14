#pragma once
#include "kissra/concepts.hpp"
#include "kissra/misc/utility.hpp"
#include "kissra/type_traits.hpp"
#include <vector>

namespace kissra {
template <typename Tag>
struct collect_mixin {
    template <template <typename...> typename TTo = std::vector, kissra::mut TSelf>
    auto collect(this TSelf&& self) {
        using val_t = kissra::iter_value_t<TSelf>;
        using container_t = TTo<val_t>;

        container_t result;
        if constexpr (kissra::is_sized_v<TSelf> && kissra::can_reserve<container_t>) {
            result.reserve(self.size());
        }

        while (auto item = self.next()) {
            if constexpr (kissra::can_push_back<container_t, val_t>) {
                result.push_back(KISSRA_FWD(*std::move(item)));
            } else if constexpr (kissra::can_insert<container_t, val_t>) {
                result.insert(std::ranges::end(result), KISSRA_FWD(*std::move(item)));
            }
        }

        return result;
    }
};
} // namespace kissra