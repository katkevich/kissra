#pragma once
#include <cstddef>
#include <type_traits>

namespace kissra {
struct advance_mixin {
    template <typename TSelf>
    auto advance(this TSelf&& self, std::size_t n) {
        return self.underlying_iter.advance(n);
    }
};
} // namespace kissra