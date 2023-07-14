//
// Created by usatiynyan on 14.07.23.
//

#include "sl/srlz/field.hpp"

#include <gtest/gtest.h>

namespace sl::srlz {

TEST(field, name) {
    using concrete_field = field<std::string, "field_name">;
    static_assert(
        concrete_field::name == std::string_view{ "field_name" },
        "field::name should be string-like and accessible in compile-time"
    );
    static_assert(sizeof(concrete_field) == sizeof(tl::optional<std::string>), "field::name should not increase size");
}

} // namespace sl::srlz
