#include "kissra/doctest_printers.hpp"

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
template <typename TBaseIter, template <typename> typename... TMixins>
class custom_enumerate_iter : public kissra::builtin_mixins<TBaseIter>, public TMixins<TBaseIter>... {
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

    auto size() const
        requires is_sized
    {
        return this->base_iter.size();
    }

private:
    TBaseIter base_iter;
    std::int64_t idx{};
};

template <typename Tag>
struct custom_enumerate_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    auto custom_enumerate(this TSelf&& self) {
        return kissra::with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... TMixins> {
            return custom_enumerate_iter<std::remove_cvref_t<TSelf>, TMixins...>{ std::forward<TSelf>(self) };
        });
    }
};

template <typename Tag>
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

/**
 * To hook into the library's mixins system and add support for your custom mixins, you need to specialize the
 * kissra::custom_mixins_traits<void> trait and define the inner `mixins` and `compose_mixins` template struct.
 * ```
 *     template <>
 *     struct kissra::registered_mixins_traits<void> {
 *         template<typename Tag>
 *         struct mixins : custom_ns::custom_enumerate_mixin<Tag>, custom_ns::custom_collect_mixin<Tag> {};
 *
 *         // if you don't care about supporting compositions - just left it with `kissra::compo::no_custom_mixins<Tag>`
 *         template <typename Tag>
 *         struct compose_mixins : kissra::compo::no_custom_mixins<Tag> {};
 *     };
 * ```
 */
template <>
struct kissra::custom_mixins_traits<void> {
    template <typename Tag>
    struct mixins : custom_ns::custom_enumerate_mixin<Tag>, custom_ns::custom_collect_mixin<Tag> {};

    template <typename Tag>
    struct compose_mixins : kissra::compo::no_custom_mixins_compose<Tag> {};
};

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

TEST_CASE(
    "composition compo::zip(arr, all().custom_enumerate()) works (custom_enumerate_mixin successfully mixed with other "
    "mixins)") {
    std::vector arr = { 1, 2, 3, 4, 5 };

    auto zip_compo = kissra::compo::zip(kissra::all(arr));
    auto actual = kissra::apply(arr, zip_compo) //
                      .custom_enumerate()
                      .members<1>()
                      .transform([](int i, int j) { return i + j; })
                      .collect();
    auto expected = std::vector{ 2, 4, 6, 8, 10 };

    REQUIRE_EQ(actual, expected);
}
} // namespace kissra::test