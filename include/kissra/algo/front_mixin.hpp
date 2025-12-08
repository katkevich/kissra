#pragma once

namespace kissra {
struct front_mixin {
    template <typename TSelf>
    auto front(this TSelf&& self) {
        return self.advance(0);
    }
};
} // namespace kissra