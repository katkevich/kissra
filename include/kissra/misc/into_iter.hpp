#pragma once
#include "kissra/concepts.hpp"
#include "kissra/iter/all_iter.hpp"
#include <concepts>
#include <utility>

namespace kissra {
template <typename TMixins, std::ranges::range TContainer>
auto into_kissra_iter(TContainer& container) {
    return all_iter<TContainer, TMixins>{ container };
}

template <typename TMixins, kissra::iterator TIter>
decltype(auto) into_kissra_iter(TIter&& iter) {
    return std::forward<TIter>(iter);
}

} // namespace kissra