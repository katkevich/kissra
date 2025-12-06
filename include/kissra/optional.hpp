#pragma once
#include "beman/optional/optional.hpp"

namespace kissra {
namespace impl {
template <typename T>
class rvalue_optional;

template <typename T>
class rvalue_optional<T&&> {
public:
    using value_type = T;

    constexpr rvalue_optional(T&& value)
        : value(std::addressof(value)) {}

    constexpr T& operator*() & noexcept {
        return *value;
    }

    constexpr const T& operator*() const& noexcept {
        return *value;
    }

    constexpr T&& operator*() && noexcept {
        return *value;
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