//
// Created by usatiynyan on 15.01.24.
//

#include "sl/srlz/to.hpp"

#include <fmt/format.h>
#include <gtest/gtest.h>
#include <range/v3/view/transform.hpp>
#include <type_traits>

namespace sl::srlz {

using JSON = std::string;

template <>
struct impl::init_to<JSON> {
    template <typename... Args>
    constexpr JSON operator()(Args&&...) const {
        return JSON{ "{" };
    }
};

template <>
struct impl::finalize_to<JSON> {
    constexpr void operator()(JSON& dst) const {
        if (dst.ends_with(',')) {
            dst.pop_back();
        }
        dst += "}";
    }
};

template <>
struct impl::assign_to<JSON> {
    template <typename field_dst_type, meta::string field_name>
    constexpr void operator()(JSON& dst, field_dst_type dst_value) const {
        operator()(dst, field_name.string_view(), dst_value);
    }

    template <typename field_dst_type>
    constexpr void operator()(JSON& dst, std::string_view field_name, field_dst_type dst_value) const {
        dst += fmt::format("\"{}\":{},", field_name, dst_value);
    }
};

template <typename T>
    requires fmt::is_formattable<T, char>::value
struct impl::to<JSON, T> {
    template <typename... Args>
    constexpr JSON operator()(const T& value, Args&&...) const {
        if constexpr (std::is_same_v<std::string, T> || std::is_same_v<char, T>) {
            return fmt::format("\"{}\"", value);
        } else {
            return fmt::format("{}", value);
        }
    }
};

template <typename T>
    requires meta::is_specialization_v<T, std::vector>
struct impl::to<JSON, T> {
    template <typename... Args>
    constexpr JSON operator()(const T& value, Args&&...) const {
        constexpr impl::to<JSON, typename T::value_type> to_f;
        const auto value_transformed = ranges::views::transform(value, [to_f](const auto& elem) { return to_f(elem); });
        return fmt::format("[{}]", fmt::join(value_transformed, ","));
    }
};

template <typename T>
    requires meta::is_specialization_v<T, std::unordered_map>
struct impl::to<JSON, T> {
    template <typename... Args>
    constexpr JSON operator()(const T& value, Args&&...) const {
        JSON dst = impl::init_to<JSON>{}();
        constexpr impl::to<JSON, typename T::mapped_type> to_f;
        constexpr impl::assign_to<JSON> assign_to_f;
        for (const auto& [key, elem] : value) {
            assign_to_f(dst, std::string_view{ key }, to_f(elem));
        }
        impl::finalize_to<JSON>{}(dst);
        return dst;
    }
};

struct TestSimpleStruct {
    field<int, "i"> i;
    field<std::vector<char>, "vc"> vc;
    field<std::string, "s"> s;
};

struct TestStruct {
    field<int, "i"> i;
    field<TestSimpleStruct, "simple"> simple;
    field<std::unordered_map<std::string, int>, "umi"> umi;
};

TEST(toFmtJson, simpleStruct) {
    TestSimpleStruct simple_object{ .i = 123, .vc = std::vector{ 'a', 'b', 'c' }, .s = "oraoraoraora" };
    const auto json_str = to<JSON>(simple_object);
    ASSERT_EQ(json_str, R"({"i":123,"vc":["a","b","c"],"s":"oraoraoraora"})");
}

TEST(toFmtJson, struct) {
    TestStruct object{ .i = 123,
                       .simple = TestSimpleStruct{ .i = 456, .vc = std::vector{ 'a', 'b', 'c' }, .s = "oraoraoraora" },
                       .umi = std::unordered_map<std::string, int>{
                           { "i_key", 7 },
                       } };
    const auto json_str = to<JSON>(object);
    ASSERT_EQ(json_str, R"({"i":123,"simple":{"i":456,"vc":["a","b","c"],"s":"oraoraoraora"},"umi":{"i_key":7}})");
}

} // namespace sl::srlz
