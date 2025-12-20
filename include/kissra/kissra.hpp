#pragma once
#include "kissra/algo/apply_mixin.hpp"
#include "kissra/algo/back_mixin.hpp"
#include "kissra/algo/collect_mixin.hpp"
#include "kissra/algo/empty_mixin.hpp"
#include "kissra/algo/find_mixin.hpp"
#include "kissra/algo/front_mixin.hpp"
#include "kissra/algo/ssize_mixin.hpp"
#include "kissra/compose.hpp"
#include "kissra/concepts.hpp"
#include "kissra/fn/cmp.hpp"
#include "kissra/fn/convert.hpp"
#include "kissra/fn/member.hpp"
#include "kissra/fn/misc.hpp"
#include "kissra/fn/num.hpp"
#include "kissra/impl/custom_mixins.hpp"
#include "kissra/iter/all_iter.hpp"
#include "kissra/iter/chunk_iter.hpp"
#include "kissra/iter/drop_iter.hpp"
#include "kissra/iter/drop_last_iter.hpp"
#include "kissra/iter/drop_last_while_iter.hpp"
#include "kissra/iter/drop_while_iter.hpp"
#include "kissra/iter/filter_iter.hpp"
#include "kissra/iter/iter_base.hpp"
#include "kissra/iter/members_iter.hpp"
#include "kissra/iter/reverse_iter.hpp"
#include "kissra/iter/transform_iter.hpp"
#include "kissra/iter/zip_iter.hpp"
#include "kissra/misc/functional.hpp"
#include "kissra/misc/optional.hpp"
#include "kissra/misc/utility.hpp"
#include "kissra/type_traits.hpp"

namespace kissra {
namespace compo {
template <typename Tag>
struct builtin_mixins : filter_compose_mixin<Tag>,
                        transform_compose_mixin<Tag>,
                        zip_compose_mixin<Tag>,
                        members_compose_mixin<Tag>,
                        reverse_compose_mixin<Tag>,
                        chunk_compose_mixin<Tag>,
                        drop_compose_mixin<Tag>,
                        drop_last_compose_mixin<Tag>,
                        drop_while_compose_mixin<Tag>,
                        drop_last_while_compose_mixin<Tag> {};
} // namespace compo

template <typename Tag>
struct builtin_mixins : filter_mixin<Tag>,
                        transform_mixin<Tag>,
                        zip_mixin<Tag>,
                        members_mixin<Tag>,
                        reverse_mixin<Tag>,
                        chunk_mixin<Tag>,
                        drop_mixin<Tag>,
                        drop_last_mixin<Tag>,
                        drop_while_mixin<Tag>,
                        drop_last_while_mixin<Tag>,
                        collect_mixin<Tag>,
                        front_mixin<Tag>,
                        apply_mixin<Tag>,
                        back_mixin<Tag>,
                        find_mixin<Tag>,
                        empty_mixin<Tag>,
                        ssize_mixin<Tag> {};

/**
 * To hook into the library's mixins system and add support for your custom mixins, you need to specialize the
 * kissra::custom_mixins_traits<void> trait and define the inner `mixins` template struct.
 * ```
 *     template <>
 *     struct kissra::registered_mixins_traits<void> {
 *         template<typename Tag>
 *         struct mixins : custom_ns::custom_enumerate_mixin<Tag>, custom_ns::custom_collect_mixin<Tag> {};
 *     };
 * ```
 */
template <typename DeferInstantiation>
struct custom_mixins_traits {
    template <typename Tag>
    using mixins = no_custom_mixins<Tag>;

    template <typename Tag>
    using compose_mixins = no_custom_mixins<Tag>;
};
} // namespace kissra