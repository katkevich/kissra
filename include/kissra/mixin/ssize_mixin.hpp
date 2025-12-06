#pragma once
#include "kissra/type_traits.hpp"
#include <vector>

namespace kissra {
struct ssize_mixin {
    template <typename TSelf>
        requires is_sized_v<TSelf>
    auto ssize(this TSelf&& self) {
        return std::make_signed_t<std::size_t>(self.size());
    }
};
} // namespace kissra