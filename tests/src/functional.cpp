#include "kissra/doctest_printers.hpp"

#include "kissra/kissra.hpp"
#include <algorithm>
#include <charconv>
#include <forward_list>
#include <iostream>
#include <list>
#include <vector>

namespace kissra::test {

using namespace std::string_literals;

TEST_CASE("'invocable_with_destructured_arg' should work with aggregates (non-ref data member)") {
    struct foo {
        std::string s;
    };
    auto fn_rvalue = [](std::string&&) {};
    auto fn_lvalue = [](std::string&) {};
    auto fn_clvalue = [](const std::string&) {};

    /* rvalue fn arg binds to rvalue */
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_rvalue), foo>);
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_rvalue), foo&&>);
    CHECK_FALSE(kissra::invocable_with_destructured_arg<decltype(fn_rvalue), foo&>);
    CHECK_FALSE(kissra::invocable_with_destructured_arg<decltype(fn_rvalue), const foo&>);

    /* non-const fn arg binds only to non-const lvalue */
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_lvalue), foo&>);
    CHECK_FALSE(kissra::invocable_with_destructured_arg<decltype(fn_lvalue), foo>);
    CHECK_FALSE(kissra::invocable_with_destructured_arg<decltype(fn_lvalue), foo&&>);
    CHECK_FALSE(kissra::invocable_with_destructured_arg<decltype(fn_lvalue), const foo&>);

    /* const fn arg binds to everything. */
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_clvalue), foo>);
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_clvalue), foo&&>);
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_clvalue), foo&>);
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_clvalue), const foo&>);
}

TEST_CASE("'invocable_with_destructured_arg' should work with aggregates (lvalue-nonconst-ref data member)") {
    struct foo {
        std::string& s;
    };
    auto fn_rvalue = [](std::string&&) {};
    auto fn_lvalue = [](std::string&) {};
    auto fn_clvalue = [](const std::string&) {};

    /* rvalue fn arg binds to rvalue (note `&& + & = &` reference collapsing rule) */
    CHECK_FALSE(kissra::invocable_with_destructured_arg<decltype(fn_rvalue), foo>);
    CHECK_FALSE(kissra::invocable_with_destructured_arg<decltype(fn_rvalue), foo&&>);
    CHECK_FALSE(kissra::invocable_with_destructured_arg<decltype(fn_rvalue), foo&>);
    CHECK_FALSE(kissra::invocable_with_destructured_arg<decltype(fn_rvalue), const foo&>);

    /* non-const fn arg binds to non-const lvalue (note `&& + & = &` reference collapsing rule) */
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_lvalue), foo&>);
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_lvalue), foo>);
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_lvalue), foo&&>);
    /* Constness of an object DO NOT propagate to the constness of a reference! Quirky, but these are C++ rules!
     * This is similar to how pointer members work. Pointer itself is const, but pointed object is not!
     */
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_lvalue), const foo&>);

    /* const fn arg binds to everything. */
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_clvalue), foo>);
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_clvalue), foo&&>);
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_clvalue), foo&>);
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_clvalue), const foo&>);
}

TEST_CASE("'invocable_with_destructured_arg' should work with std::tuple") {
    auto fn_rvalue = [](std::string&&) {};
    auto fn_lvalue = [](std::string&) {};
    auto fn_clvalue = [](const std::string&) {};

    using tpl = std::tuple<std::string>;

    /* rvalue fn arg binds to rvalue */
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_rvalue), tpl>);
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_rvalue), tpl&&>);
    CHECK_FALSE(kissra::invocable_with_destructured_arg<decltype(fn_rvalue), tpl&>);
    CHECK_FALSE(kissra::invocable_with_destructured_arg<decltype(fn_rvalue), const tpl&>);

    /* non-const fn arg binds only to non-const lvalue */
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_lvalue), tpl&>);
    CHECK_FALSE(kissra::invocable_with_destructured_arg<decltype(fn_lvalue), tpl>);
    CHECK_FALSE(kissra::invocable_with_destructured_arg<decltype(fn_lvalue), tpl&&>);
    CHECK_FALSE(kissra::invocable_with_destructured_arg<decltype(fn_lvalue), const tpl&>);

    /* const fn arg binds to everything. */
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_clvalue), tpl>);
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_clvalue), tpl&&>);
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_clvalue), tpl&>);
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_clvalue), const tpl&>);
}

TEST_CASE("'invocable_with_destructured_arg' should work with std::tuple (rvalue-ref member)") {
    auto fn_rvalue = [](std::string&&) {};
    auto fn_lvalue = [](std::string&) {};
    auto fn_clvalue = [](const std::string&) {};

    using tpl = std::tuple<std::string&&>;

    /* rvalue fn arg binds to rvalue */
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_rvalue), tpl>);
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_rvalue), tpl&&>);
    CHECK_FALSE(kissra::invocable_with_destructured_arg<decltype(fn_rvalue), tpl&>);
    CHECK_FALSE(kissra::invocable_with_destructured_arg<decltype(fn_rvalue), const tpl&>);

    /* non-const fn arg binds only to non-const lvalue */
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_lvalue), tpl&>);
    CHECK_FALSE(kissra::invocable_with_destructured_arg<decltype(fn_lvalue), tpl>);
    CHECK_FALSE(kissra::invocable_with_destructured_arg<decltype(fn_lvalue), tpl&&>);
    /* Constness of an object DO NOT propagate to the constness of a reference! Quirky, but these are C++ rules!
     * This is similar to how pointer members work. Pointer itself is const, but pointed object is not!
     */
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_lvalue), const tpl&>);

    /* const fn arg binds to everything. */
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_clvalue), tpl>);
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_clvalue), tpl&&>);
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_clvalue), tpl&>);
    CHECK(kissra::invocable_with_destructured_arg<decltype(fn_clvalue), const tpl&>);
}
} // namespace kissra::test