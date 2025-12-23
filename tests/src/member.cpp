#include "kissra/doctest_printers.hpp"

#include "kissra/kissra.hpp"
#include "kissra/noisy.hpp"
#include <algorithm>
#include <charconv>
#include <forward_list>
#include <iostream>
#include <list>
#include <type_traits>
#include <vector>

namespace kissra::test {
using namespace std::string_literals;

TEST_CASE("transform(<producing lvalue refs>).transform(member<0>) should produce lvalue refs (no copies)") {

    struct Foo {
        noisy member;
    };
    test::tracker tracker;
    std::array arr = { Foo{ noisy{ tracker } } };

    auto iter = kissra::all(arr)
                    /* References all the way through. No copies or moves here. */
                    .transform(fn::member<0>);
    std::ignore = iter.next();

    static_assert(std::is_same_v<typename decltype(iter)::reference, noisy&>);

    REQUIRE_EQ(tracker.copy_ctor, 0);
    REQUIRE_EQ(tracker.move_ctor, 0);
    REQUIRE_EQ(tracker.copy_op, 0);
    REQUIRE_EQ(tracker.move_op, 0);
}

TEST_CASE(
    "transform(<producing const lvalue refs>).transform(member<0>) should produce const lvalue refs (const "
    "propagation, no copies)") {

    struct Foo {
        noisy member;
    };
    test::tracker tracker;
    const std::array arr = { Foo{ noisy{ tracker } } };

    auto iter = kissra::all(arr)
                    /* References all the way through. No copies or moves here. */
                    .transform(fn::member<0>);
    std::ignore = iter.next();

    /* Underlying container is const hence `const noisy&` here. */
    static_assert(std::is_same_v<typename decltype(iter)::reference, const noisy&>);

    REQUIRE_EQ(tracker.copy_ctor, 0);
    REQUIRE_EQ(tracker.move_ctor, 0);
    REQUIRE_EQ(tracker.copy_op, 0);
    REQUIRE_EQ(tracker.move_op, 0);
}

TEST_CASE("transform(<producing values>).transform(member<0>) should produce values (x3 move ctor)") {

    struct Foo {
        noisy member;
    };
    test::tracker tracker;
    std::array arr = { 1 };

    auto iter = kissra::all(arr)
                    /* Copy-elision (no move) + move-initialize `optional` (+1 move) */
                    .transform([&](int) { return Foo(tracker); })
                    /**
                     * Move `Foo::member' out of `fn::member` functor (+1 move) + move-initialize `optional` (+1 move)
                     * NRVO or RVO ain't kick in `fn::member` coz (N)RVO ain't working for "members" or "function args".
                     */
                    .transform(fn::member<0>);
    std::ignore = iter.next();

    static_assert(std::is_same_v<typename decltype(iter)::reference, noisy>);

    REQUIRE_EQ(tracker.copy_ctor, 0);
    REQUIRE_EQ(tracker.move_ctor, 3);
    REQUIRE_EQ(tracker.copy_op, 0);
    REQUIRE_EQ(tracker.move_op, 0);
}

TEST_CASE("transform(<producing rvalue refs>).transform(member<0>) should produce values (x2 move ctor)") {

    struct Foo {
        noisy member;
    };
    test::tracker tracker;
    std::array arr = { Foo{ noisy{ tracker } } };
    tracker.reset();

    auto iter = kissra::all(arr)
                    /* No copy or moves here (`optional<Foo&&>` is being returned out of this `transform`) */
                    .transform([&](Foo& foo) -> decltype(auto) { return std::move(foo); })
                    /**
                     * Move `Foo::member' out of `fn::member` functor (+1 move) + move-initialize `optional` (+1 move)
                     * NRVO or RVO ain't kick in `fn::member` coz (N)RVO ain't working for "members" or "function args".
                     * Basically, rvalue refs are being treated the same as just values.
                     */
                    .transform(fn::member<0>);
    std::ignore = iter.next();

    static_assert(std::is_same_v<typename decltype(iter)::reference, noisy>);

    REQUIRE_EQ(tracker.copy_ctor, 0);
    REQUIRE_EQ(tracker.move_ctor, 2);
    REQUIRE_EQ(tracker.copy_op, 0);
    REQUIRE_EQ(tracker.move_op, 0);
}
} // namespace kissra::test