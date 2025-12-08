#include <doctest/doctest.h>

#include "kissra/kissra.hpp"
#include <algorithm>
#include <deque>
#include <format>
#include <forward_list>
#include <iostream>
#include <list>
#include <set>
#include <tuple>
#include <type_traits>
#include <unordered_set>
#include <vector>

namespace custom_ns {
template <typename TUnderlyingIter, typename TMixins>
/**
 * `size_mixin` isn't builtin coz you cannot implement it generically for ANY kind of iterators in O(1)
 * It may require custom logic in you iterator (see `drop_iterator` for instance).
 *
 * But, for iterators which DO NOT filter items in any way (`enumerate`/`transform` etc.) it is enough to just delegate
 * `size` functionality to underlying iterator (which is exactly what `kissra::size_mixin` does)
 */
class custom_enumerate_iter : public kissra::size_mixin, public TMixins {
    /* Should be friend to access `underlying_iter` */
    friend class kissra::size_mixin;

public:
    using value_type = std::tuple<std::int64_t, typename TUnderlyingIter::reference>;
    using reference = std::tuple<std::int64_t, typename TUnderlyingIter::reference>;
    using const_reference = std::tuple<std::int64_t, typename TUnderlyingIter::const_reference>;

    template <typename TSelf>
    using ref_t = std::conditional_t<std::is_const_v<std::remove_reference_t<TSelf>>, const_reference, reference>;

    template <typename TSelf>
    using result_t = kissra::optional<ref_t<TSelf>>;

    /**
     * Add support for "size" to illustrate the possibility to intermix custom mixins with builtin ones.
     * I.e. you can call builtin mixin method on your custom iterators (`custom_enumerate().size()`).
     */
    static constexpr bool is_sized = true;
    /**
     * Using 'false' here for simplicity
     * Just "give me the next element" support (basically input iterator).
     */
    static constexpr bool is_common = false;
    static constexpr bool is_forward = false;
    static constexpr bool is_bidir = false;
    static constexpr bool is_random = false;

    template <typename UUnderlyingIter>
    custom_enumerate_iter(UUnderlyingIter&& underlying_iter)
        : underlying_iter(std::forward<UUnderlyingIter>(underlying_iter)) {}

    template <typename TSelf>
    result_t<TSelf> next(this TSelf&& self) {
        if (auto item = self.underlying_iter.next()) {
            return ref_t<TSelf>{ self.idx++, *item };
        }
        return {};
    }

    template <typename TSelf>
    result_t<TSelf> advance(this TSelf&& self, std::size_t n) {
        if (auto item = self.underlying_iter.advance(n)) {
            self.idx += n;
            return ref_t<TSelf>{ self.idx, *item };
        }
        return {};
    }

private:
    TUnderlyingIter underlying_iter;
    std::int64_t idx{};
};

struct custom_enumerate_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    auto custom_enumerate(this TSelf&& self) {
        auto mixins = kissra::registered_mixins<DeferInstantiation>();
        return custom_enumerate_iter<std::remove_cvref_t<TSelf>, decltype(mixins)>{ std::forward<TSelf>(self) };
    }
};

struct custom_collect_mixin {
    template <template <typename...> typename TTo = std::vector, typename TSelf>
    auto custom_push_back(this TSelf&& self) {
        using val_t = kissra::iter_value_t<TSelf>;
        using container_t = TTo<val_t>;

        container_t result;
        while (auto item = self.next()) {
            result.push_back(KISSRA_FWD(*std::move(item)));
        }

        return result;
    }
};
} // namespace custom_ns

/* Just register it with a macro before any kissra iterator instantiation in current TU.
 *
 * If you're alergic to macros or you're using modularized target (hopefully it will be done soon)
 * then you can specialize mixins traits instead (basically the guts of a macro):
 *
 * ```
 *   template <>
 *   struct kissra::registered_mixins_traits<void> {
 *       struct type : kissra::builtin_mixins, custom_ns::custom_enumerate_mixin, custom_ns::custom_collect_mixin {};
 *   };
 * ```
 */
KISSRA_REGISTER_CUSTOM_MIXINS(custom_ns::custom_enumerate_mixin, custom_ns::custom_collect_mixin)

namespace kissra::test {
using namespace std::string_literals;

TEST_CASE("all().custom_enumerate() works") {
    std::array arr = { "0"s, "1"s, "2"s, "3"s };
    auto iter = kissra::all(arr).custom_enumerate();

    while (auto item = iter.next()) {
        REQUIRE_EQ(std::to_string(std::get<0>(*item)), std::get<1>(*item));
    }
}

TEST_CASE("all().custom_enumerate().size() works (custom_enumerate_mixin successfully mixed with other mixins)") {
    std::array arr = { "0"s, "1"s, "2"s, "3"s };
    auto size = kissra::all(arr).custom_enumerate().size();

    REQUIRE_EQ(size, 4);
}

TEST_CASE(
    "all().custom_enumerate().filter(...).transform(...).collect() works (custom_enumerate_mixin successfully mixed "
    "with other mixins)") {
    std::vector arr = { "0"s, "1"s, "2"s, "3"s };

    auto actual = kissra::all(arr)
                      .custom_enumerate()
                      .filter([](std::tuple<std::int64_t, std::string&> item) { return std::get<0>(item) > 1; })
                      .transform([](auto item) { return std::get<1>(item); })
                      .custom_push_back();
    REQUIRE_EQ(actual, (std::vector{ "2"s, "3"s }));
}
} // namespace kissra::test