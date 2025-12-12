#pragma once

namespace kissra::tmp {
template <typename... Ts>
struct type_list {};

template <typename... Ts>
struct front;

template <typename T, typename... Ts>
struct front<T, Ts...> {
    using type = T;
};

template <typename... Ts>
using front_t = typename front<Ts...>::type;
} // namespace kissra::tmp