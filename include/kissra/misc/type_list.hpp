#pragma once
#include "kissra/impl/export.hpp"

#ifndef KISSRA_MODULE
#endif

KISSRA_EXPORT()
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

template <typename TList, typename T>
struct append;

template <typename... Ts, typename T>
struct append<type_list<Ts...>, T> {
    using type = type_list<Ts..., T>;
};

template <typename... Ts>
using append_t = typename append<Ts...>::type;
} // namespace kissra::tmp