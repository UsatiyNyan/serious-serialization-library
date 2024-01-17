#include "sl/srlz/ext/nlohmann_json.hpp"
#include "sl/srlz/field.hpp"

#include <fmt/format.h>
#include <gtest/gtest.h>

namespace sl::srlz {

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

TEST(toJson, simpleStruct) {
    TestSimpleStruct simple_object{ .i = 123, .vc = std::vector{ 'a', 'b', 'c' }, .s = "oraoraoraora" };
    const auto json = to<nlohmann::json>(simple_object);
    const auto check_json = nlohmann::json::parse(R"({"i":123,"vc":[97,98,99],"s":"oraoraoraora"})");
    ASSERT_EQ(json, check_json);
}

TEST(toJson, simpleStructSkipFields) {
    TestSimpleStruct simple_object{ .i = tl::nullopt, .vc = std::vector{ 'a', 'b', 'c' }, .s = tl::nullopt };
    const auto json = to<nlohmann::json>(simple_object);
    const auto check_json = nlohmann::json::parse(R"({"vc":[97,98,99]})");
    ASSERT_EQ(json, check_json);
}

TEST(toJson, simpleStructEmptyVec) {
    TestSimpleStruct simple_object{ .i = 123, .vc = std::vector<char>{}, .s = "oraoraoraora" };
    const auto json = to<nlohmann::json>(simple_object);
    const auto check_json = nlohmann::json::parse(R"({"i":123,"vc":[],"s":"oraoraoraora"})");
    ASSERT_EQ(json, check_json);
}

TEST(toJson, struct) {
    TestStruct object{ .i = 123,
                       .simple = TestSimpleStruct{ .i = 456, .vc = std::vector{ 'a', 'b', 'c' }, .s = "oraoraoraora" },
                       .mi = std::map<std::string, int>{
                           { "i_key", 7 },
                           { "j_key", 13 },
                       } };
    const auto json = to<nlohmann::json>(object);
    const auto check_json = nlohmann::json::parse(
        R"({"i":123,"simple":{"i":456,"vc":[97,98,99],"s":"oraoraoraora"},"mi":{"i_key":7,"j_key":13}})"
    );
    ASSERT_EQ(json, check_json);
}

TEST(toJson, structSkipFields) {
    TestStruct object{ .i = 123,
                       .simple = TestSimpleStruct{ .i = 456, .vc = std::vector{ 'a', 'b', 'c' }, .s = tl::nullopt },
                       .mi = tl::nullopt };
    const auto json = to<nlohmann::json>(object);
    const auto check_json = nlohmann::json::parse(R"({"i":123,"simple":{"i":456,"vc":[97,98,99]}})");
    ASSERT_EQ(json, check_json);
}

TEST(toJson, structEmptyMap) {
    TestStruct object{ .i = 123,
                       .simple = TestSimpleStruct{ .i = 456, .vc = std::vector{ 'a', 'b', 'c' }, .s = "oraoraoraora" },
                       .mi = std::map<std::string, int>{} };
    const auto json = to<nlohmann::json>(object);
    const auto check_json =
        nlohmann::json::parse(R"({"i":123,"simple":{"i":456,"vc":[97,98,99],"s":"oraoraoraora"},"mi":{}})");
    ASSERT_EQ(json, check_json);
}

} // namespace sl::srlz
