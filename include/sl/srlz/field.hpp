//
// Created by usatiynyan on 14.07.23.
//

#pragma once

#include "sl/meta/string.hpp"

#include <tl/optional.hpp>

namespace sl::srlz {

template <typename T, meta::string name_>
struct field : tl::optional<T> {
    static constexpr auto name = name_.chars;
    using tl::optional<T>::optional;
    using tl::optional<T>::operator=;
    bool operator==(const field&) const = default;
};

} // namespace sl::srlz
