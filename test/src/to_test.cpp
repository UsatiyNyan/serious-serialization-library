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
    template <typename SrcT, typename... Args>
    constexpr JSON operator()(const SrcT&, Args&&...) const {
        if constexpr (meta::is_range<SrcT> && !meta::is_mapping_v<SrcT>) {
            return "[";
        } else {
            return "{";
        }
    }
};

template <>
struct impl::finalize_to<JSON> {
    template <typename SrcT>
    constexpr void operator()(JSON& dst, const SrcT&) const {
        if (dst.ends_with(',')) {
            dst.pop_back();
        }
        if constexpr (meta::is_range<SrcT> && !meta::is_mapping_v<SrcT>) {
            dst += "]";
        } else {
            dst += "}";
        }
    }
};

template <>
struct impl::assign_to<JSON> {
    // mapping assignment
    constexpr void operator()(JSON& dst, std::string_view field_name, JSON dst_value) const {
        dst += fmt::format("\"{}\":{},", field_name, dst_value);
    }

    // range assignment
    constexpr void operator()(JSON& dst, std::size_t, JSON dst_value) const { dst += (dst_value + ','); }
};

template <typename SrcT>
    requires fmt::is_formattable<SrcT, char>::value
struct impl::to<JSON, SrcT> {
    template <typename... Args>
    constexpr JSON operator()(const SrcT& src, Args&&...) const {
        if constexpr (std::is_same_v<std::string, SrcT> || std::is_same_v<char, SrcT>) {
            return fmt::format("\"{}\"", src);
        } else {
            return fmt::format("{}", src);
        }
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
    field<std::map<std::string, int>, "mi"> mi;
};

TEST(toFmtJson, simpleStruct) {
    TestSimpleStruct simple_object{ .i = 123, .vc = std::vector{ 'a', 'b', 'c' }, .s = "oraoraoraora" };
    const auto json_str = to<JSON>(simple_object);
    ASSERT_EQ(json_str, R"({"i":123,"vc":["a","b","c"],"s":"oraoraoraora"})");
}

TEST(toFmtJson, simpleStructSkipFields) {
    TestSimpleStruct simple_object{ .i = tl::nullopt, .vc = std::vector{ 'a', 'b', 'c' }, .s = tl::nullopt };
    const auto json_str = to<JSON>(simple_object);
    ASSERT_EQ(json_str, R"({"vc":["a","b","c"]})");
}

TEST(toFmtJson, simpleStructEmptyVec) {
    TestSimpleStruct simple_object{ .i = 123, .vc = std::vector<char>{}, .s = "oraoraoraora" };
    const auto json_str = to<JSON>(simple_object);
    ASSERT_EQ(json_str, R"({"i":123,"vc":[],"s":"oraoraoraora"})");
}

TEST(toFmtJson, struct) {
    TestStruct object{ .i = 123,
                       .simple = TestSimpleStruct{ .i = 456, .vc = std::vector{ 'a', 'b', 'c' }, .s = "oraoraoraora" },
                       .mi = std::map<std::string, int>{
                           { "i_key", 7 },
                           { "j_key", 13 },
                       } };
    const auto json_str = to<JSON>(object);
    ASSERT_EQ(
        json_str, R"({"i":123,"simple":{"i":456,"vc":["a","b","c"],"s":"oraoraoraora"},"mi":{"i_key":7,"j_key":13}})"
    );
}

TEST(toFmtJson, structSkipFields) {
    TestStruct object{ .i = 123,
                       .simple = TestSimpleStruct{ .i = 456, .vc = std::vector{ 'a', 'b', 'c' }, .s = tl::nullopt },
                       .mi = tl::nullopt };
    const auto json_str = to<JSON>(object);
    ASSERT_EQ(json_str, R"({"i":123,"simple":{"i":456,"vc":["a","b","c"]}})");
}

TEST(toFmtJson, structEmptyMap) {
    TestStruct object{ .i = 123,
                       .simple = TestSimpleStruct{ .i = 456, .vc = std::vector{ 'a', 'b', 'c' }, .s = "oraoraoraora" },
                       .mi = std::map<std::string, int>{} };
    const auto json_str = to<JSON>(object);
    ASSERT_EQ(json_str, R"({"i":123,"simple":{"i":456,"vc":["a","b","c"],"s":"oraoraoraora"},"mi":{}})");
}

} // namespace sl::srlz
