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
template <typename TBaseIter, typename TMixins>
/**
 * `size_mixin` isn't builtin coz you cannot implement it generically for ANY kind of iterators in O(1)
 * It may require custom logic in you iterator (see `drop_iterator` for instance).
 *
 * But, for iterators which DO NOT filter items in any way (`enumerate`/`transform` etc.) it is enough to just delegate
 * `size` functionality to base iterator (which is exactly what `kissra::size_mixin` does)
 */
class custom_enumerate_iter : public kissra::size_mixin, public TMixins {
    /* Should be friend to access `base_iter` */
    friend class kissra::size_mixin;

public:
    using value_type = std::tuple<std::int64_t, typename TBaseIter::reference>;
    using reference = std::tuple<std::int64_t, typename TBaseIter::reference>;
    using result_t = kissra::optional<reference>;

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

    template <typename UBaseIter>
    custom_enumerate_iter(UBaseIter&& base_iter)
        : base_iter(std::forward<UBaseIter>(base_iter)) {}

    result_t next() {
        if (auto item = this->base_iter.next()) {
            return reference{ this->idx++, *item };
        }
        return {};
    }

    result_t nth(std::size_t n) {
        if (auto item = this->base_iter.nth(n)) {
            this->idx += n;
            return reference{ this->idx, *item };
        }
        return {};
    }

private:
    TBaseIter base_iter;
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
    template <typename TSelf, template <typename...> typename TTo = std::vector>
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