#pragma once
#include <cstddef>
#include <utility>

#define KISSRA_FWD(x) std::forward<decltype(x)>(x)

namespace kissra::impl {

/**
 * Keep track how much mixin were registered using macro from below.
 * Each mixin provides an implementation for a particular iterator (see filter_iter & filter_mixin as an example).
 * "DeferInstantiation" allows us to use these functions while they are not yet defined (defered instantiation).
 */
template <typename DeferInstantiation = void>
constexpr std::size_t mixins_count();

/**
 * See "loophole" hack: https://alexpolt.github.io/type-loophole.html
 * Each `mixin_tag<Idx>` binds to a particular mixin. `mixin_tag` is unique for each mixin.
 */
template <std::size_t N>
struct mixin_tag {};

namespace {
template <typename TMixin>
constexpr std::size_t mixin_idx{};
}
} // namespace kissra::impl

/**
 * CRITICAL: All translation units must register the EXACT SAME mixins in the EXACT SAME ORDER using these macros. Otherwise, ODR violations
 * will occur and behavior is undefined. So it is better to move registration logic into a separate header file and use it across all codebase.
 *
 * Example usage (must be identical in every .cpp file):
 *   KISSRA_REGISTER_MIXINS_BEGIN
 *   KISSRA_REGISTER_BUILTIN_MIXINS()
 *   KISSRA_REGISTER_MIXIN(user_ns::user_custom_mixin_1)
 *   KISSRA_REGISTER_MIXIN(user_ns::user_custom_mixin_2)
 *   KISSRA_REGISTER_MIXINS_END
 */

#define KISSRA_REGISTER_MIXINS_BEGIN                                  \
    namespace kissra::impl {                                          \
    template <typename TMixin, std::size_t N>                         \
    struct loophole_t {                                               \
        friend auto loophole(mixin_tag<N>) {                          \
            return TMixin{};                                          \
        };                                                            \
    };                                                                \
    /* anonimous namespace prevent ODR violations */                  \
    namespace {                                                       \
    static constexpr std::size_t mixins_count_baseline = __COUNTER__; \
    }                                                                 \
    }

#define KISSRA_REGISTER_MIXIN(Mixin)                                                                          \
    namespace kissra::impl {                                                                                  \
    /* We must ensure that `mixin_idx<Mixin>` have the same value across all TUs to prevent ODR violations */ \
    /* thus the `mixins_count_baseline` usage (in this case delta should always be the same) */               \
    namespace {                                                                                               \
    template <>                                                                                               \
    constexpr std::size_t mixin_idx<Mixin> = __COUNTER__ - mixins_count_baseline - 1;                         \
    }                                                                                                         \
    auto loophole(mixin_tag<mixin_idx<Mixin>>);                                                               \
    static_assert(sizeof(loophole_t<Mixin, mixin_idx<Mixin>>));                                               \
    }

#define KISSRA_REGISTER_MIXINS_END                                                                                  \
    namespace kissra::impl {                                                                                        \
    template <>                                                                                                     \
    constexpr std::size_t mixins_count<>() {                                                                        \
        /* We must ensure that `mixins_count<>()` return the same value across all TUs to prevent ODR violations */ \
        /* thus the `mixins_count_baseline` usage (in this case delta should always be the same) */                 \
        return __COUNTER__ - mixins_count_baseline - 1;                                                             \
    }                                                                                                               \
    }

#define KISSRA_REGISTER_BUILTIN_MIXINS              \
    KISSRA_REGISTER_MIXIN(kissra::filter_mixin)     \
    KISSRA_REGISTER_MIXIN(kissra::transform_mixin)  \
    KISSRA_REGISTER_MIXIN(kissra::reverse_mixin)    \
    KISSRA_REGISTER_MIXIN(kissra::drop_mixin)       \
    KISSRA_REGISTER_MIXIN(kissra::drop_last_mixin)  \
    KISSRA_REGISTER_MIXIN(kissra::drop_while_mixin) \
    KISSRA_REGISTER_MIXIN(kissra::drop_last_while_mixin)

namespace kissra {
/**
 * Get all registered mixin types. You will need this list while creating your own custom_iter & custom_mixin.
 */
template <typename DeferInstantiation = void>
auto registered_mixins() {
    constexpr auto mixins_count = impl::mixins_count<DeferInstantiation>();

    return []<auto... Is>(std::index_sequence<Is...>) {
        /**
         * Using static tag with per-mixin unique index we extract tha associated mixin instance from the compiler
         * state. See "loophole" hack: https://alexpolt.github.io/type-loophole.html
         */
        return std::tuple<decltype(loophole(impl::mixin_tag<Is>{}))...>{};
    }(std::make_index_sequence<mixins_count>{});
}
} // namespace kissra