#pragma once
#include "beman/optional/optional.hpp"

namespace kissra {
using nullopt_t = beman::optional::nullopt_t;
constexpr auto nullopt = beman::optional::nullopt;

namespace impl {
template <typename T>
class rvalue_optional;

template <typename T>
class rvalue_optional<T&&> {
public:
    using value_type = T;

    constexpr rvalue_optional() noexcept {}
    constexpr rvalue_optional(nullopt_t) noexcept {}

    constexpr rvalue_optional(T&& value) noexcept
        : value(std::addressof(value)) {}

    constexpr bool has_value() const noexcept {
        return value != nullptr;
    }

    constexpr T& operator*() & noexcept {
        return *value;
    }

    constexpr const T& operator*() const& noexcept {
        return *value;
    }

    constexpr T&& operator*() && noexcept {
        return *value;
    }

    constexpr T* operator->() noexcept {
        return value;
    }

    constexpr const T* operator->() const noexcept {
        return value;
    }

    constexpr operator bool() const {
        return value;
    }

private:
    T* value{};
};

template <typename T>
struct optional {
    using type = beman::optional::optional<T>;
};

template <typename T>
struct optional<T&&> {
    using type = impl::rvalue_optional<T&&>;
};
} // namespace impl

template <typename T>
using optional = typename impl::optional<T>::type;
} // namespace kissra
