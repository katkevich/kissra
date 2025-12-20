#pragma once
#include "kissra/impl/export.hpp"
#include "kissra/misc/type_list.hpp"
#include "kissra/misc/utility.hpp"

#ifndef KISSRA_MODULE
#include <type_traits>
#endif

KISSRA_EXPORT()
namespace kissra {
template <typename TIt>
struct iter_value {
    using type = typename std::remove_reference_t<TIt>::value_type;
};

template <typename TIt>
struct iter_reference {
    using type = typename std::remove_reference_t<TIt>::reference;
};

template <typename TIt>
struct iter_result {
    using type = typename std::remove_reference_t<TIt>::result_t;
};

template <typename TIt>
using iter_value_t = typename iter_value<TIt>::type;

template <typename TIt>
using iter_reference_t = typename iter_reference<TIt>::type;

template <typename TIt>
using iter_result_t = typename iter_result<TIt>::type;

template <typename T>
constexpr bool is_sized_v = std::remove_cvref_t<T>::is_sized;

template <typename T>
constexpr bool is_common_v = std::remove_cvref_t<T>::is_common;

template <typename T>
constexpr bool is_forward_v = std::remove_cvref_t<T>::is_forward;

template <typename T>
constexpr bool is_bidir_v = std::remove_cvref_t<T>::is_bidir;

template <typename T>
constexpr bool is_random_v = std::remove_cvref_t<T>::is_random;


/**
 * TArg   + T&& -> T&&
 * TArg   + T&  -> T&
 * TArg   + T   -> T&&
 * TArg&& + T&& -> T&&
 * TArg&& + T&  -> T&
 * TArg&& + T   -> T&&
 * TArg&  + T&& -> T&
 * TArg&  + T&  -> T&
 * TArg&  + T   -> T&
 */
template <typename TArg>
struct destructured_members_type_list {
    static consteval auto detect_destructured_members(TArg&& arg) {
        auto&& [... xs] = arg;
        return tmp::type_list<decltype(kissra::forward_member<TArg, decltype(xs)>(xs))...>{};
    }

    using type = decltype(detect_destructured_members(std::declval<TArg>()));
};

template <typename TArg>
using destructured_members_type_list_t = typename destructured_members_type_list<TArg>::type;


/**
 * `kissra::is_invocable_with_type_list<TFn, tmp::type_list<TArgs...>>` is equivalent to `std::is_invocable<TFn, TArgs...>`
 */
template <typename TFn, typename TArgsTypeList>
struct is_invocable_with_type_list : std::false_type {};

template <typename TFn, typename... Ts>
struct is_invocable_with_type_list<TFn, tmp::type_list<Ts...>> : std::bool_constant<std::is_invocable_v<TFn, Ts...>> {};

template <typename TFn, typename TArgsTypeList>
constexpr bool is_invocable_with_type_list_v = is_invocable_with_type_list<TFn, TArgsTypeList>::value;


/**
 * `kissra::invoke_with_type_list_result<TFn, tmp::type_list<TArgs...>>` is equivalent to `std::invoke_result<TFn, TArgs...>`
 */
template <typename TFn, typename TArgsTypeList>
struct invoke_with_type_list_result;

template <typename TFn, typename... Ts>
struct invoke_with_type_list_result<TFn, tmp::type_list<Ts...>> {
    using type = std::invoke_result_t<TFn, Ts...>;
};
template <typename TFn, typename TArgsTypeList>
using invoke_with_type_list_result_t = typename invoke_with_type_list_result<TFn, TArgsTypeList>::type;


/**
 * `kissra::invoke_with_destructured_arg_result<TFn, TAggregate>` is equivalent to `std::invoke_result<TFn, TAggregate::members...>`
 * `TArg` should be an aggregate or should implement tuple protocol (`std::tuple_size<...>` & `std::tuple_element<...>`)
 */
template <typename TFn, typename TArg>
struct invoke_with_destructured_arg_result {
    using type = invoke_with_type_list_result_t<TFn, destructured_members_type_list_t<TArg>>;
};
template <typename TFn, typename TArg>
using invoke_with_destructured_arg_result_t = typename invoke_with_destructured_arg_result<TFn, TArg>::type;


template <typename TFn, typename... TArgs>
struct invoke_result {
    using type = typename std::conditional_t<std::is_invocable_v<TFn, TArgs...> || (sizeof...(TArgs) > 1),
        std::invoke_result<TFn, TArgs...>,
        invoke_with_destructured_arg_result<TFn, tmp::front_t<TArgs...>>>::type;
};
template <typename TFn, typename... TArgs>
using invoke_result_t = typename invoke_result<TFn, TArgs...>::type;
} // namespace kissra