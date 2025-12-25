#pragma once
#include "kissra/concepts.hpp"
#include "kissra/impl/export.hpp"
#include "kissra/type_traits.hpp"

#ifndef KISSRA_MODULE
#endif

KISSRA_EXPORT()
namespace kissra {
template <typename Tag>
struct empty_mixin {
    template <kissra::mut TSelf>
    constexpr bool empty(this TSelf&& self) {
        // self.advance(0);
        // return self.underlying_subrange().empty();

        // TODO: try make it non-advancing (using cursor & sentinel - could be tricky, given different cursor & sentinel types out there: take, zip, reverse to name a few)
        return self.advance(1) == 0;
    }

    template <typename TSelf>
        requires is_sized_v<TSelf>
    constexpr bool empty(this const TSelf& self) {
        return self.size() == 0;
    }
};
} // namespace kissra