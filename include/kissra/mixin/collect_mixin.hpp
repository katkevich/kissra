#pragma once
#include "kissra/concepts.hpp"
#include "kissra/misc/fwd.hpp"
#include "kissra/type_traits.hpp"
#include <vector>

namespace kissra {
struct collect_mixin {
    template <template <typename...> typename TTo = std::vector, typename TSelf>
    auto collect(this TSelf&& self) {
        using val_t = iter_value_t<TSelf>;
        using result_t = TTo<val_t>;

        result_t result;
        // TODO: add reserve for 'sized' ranges

        while (auto item = self.next()) {
            if constexpr (can_push_back<result_t, val_t>) {
                result.push_back(KISSRA_FWD(*std::move(item)));
            } else if constexpr (can_insert<result_t, val_t>) {
                result.insert(std::ranges::end(result), KISSRA_FWD(*std::move(item)));
            }
        }

        return result;
    }
};
} // namespace kissra