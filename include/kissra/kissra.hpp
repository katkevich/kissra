#pragma once
#include "kissra/algo/collect_mixin.hpp"
#include "kissra/algo/size_mixin.hpp"
#include "kissra/algo/ssize_mixin.hpp"
#include "kissra/concepts.hpp"
#include "kissra/fn/cmp.hpp"
#include "kissra/fn/convert.hpp"
#include "kissra/fn/num.hpp"
#include "kissra/iter/all_iter.hpp"
#include "kissra/iter/drop_iter.hpp"
#include "kissra/iter/drop_last_iter.hpp"
#include "kissra/iter/drop_last_while_iter.hpp"
#include "kissra/iter/drop_while_iter.hpp"
#include "kissra/iter/filter_iter.hpp"
#include "kissra/iter/reverse_iter.hpp"
#include "kissra/iter/transform_iter.hpp"
#include "kissra/misc/optional.hpp"
#include "kissra/registered_mixins_fwd.hpp"
#include "kissra/type_traits.hpp"

namespace kissra {
struct builtin_mixins : filter_mixin,
                        transform_mixin,
                        reverse_mixin,
                        drop_mixin,
                        drop_last_mixin,
                        drop_while_mixin,
                        drop_last_while_mixin,
                        collect_mixin,
                        ssize_mixin {};

template <typename T>
struct registered_mixins_traits {
    using type = builtin_mixins;
};

template <typename DeferInstantiation = void>
auto registered_mixins() {
    return typename registered_mixins_traits<DeferInstantiation>::type{};
}
} // namespace kissra


#define KISSRA_REGISTER_CUSTOM_MIXINS(...)                    \
    namespace kissra {                                        \
    struct custom_mixins __VA_OPT__( :) __VA_ARGS__{};        \
    struct mixins : kissra::builtin_mixins, custom_mixins {}; \
    template <>                                               \
    struct registered_mixins_traits<void> {                   \
        using type = mixins;                                  \
    };                                                        \
    }