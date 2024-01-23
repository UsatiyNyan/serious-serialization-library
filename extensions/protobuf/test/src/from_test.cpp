#include "sl/srlz/ext/protobuf.hpp"

#include "fixtures.hpp"

#include <fmt/format.h>
#include <gtest/gtest.h>

namespace sl::srlz {

// TEST(fromPb, simpleStruct) {
//     const auto json = nlohmann::json::parse(R"({"i":123,"vc":[97,98,99],"s":"oraoraoraora"})");
//     const auto simple_object = from<TestSimpleStruct>(json);
//     const TestSimpleStruct check_object{ .i = 123, .vc = std::vector{ 'a', 'b', 'c' }, .s = "oraoraoraora" };
//     ASSERT_EQ(simple_object, check_object);
// }
//
// TEST(fromPb, simpleStructSkipFields) {
//     const auto json = nlohmann::json::parse(R"({"vc":[97,98,99]})");
//     const auto simple_object = from<TestSimpleStruct>(json);
//     const TestSimpleStruct check_object{ .i = tl::nullopt, .vc = std::vector{ 'a', 'b', 'c' }, .s = tl::nullopt };
//     ASSERT_EQ(simple_object, check_object);
// }
//
// TEST(fromPb, simpleStructEmptyVec) {
//     const auto json = nlohmann::json::parse(R"({"i":123,"vc":[],"s":"oraoraoraora"})");
//     const auto simple_object = from<TestSimpleStruct>(json);
//     const TestSimpleStruct check_object{ .i = 123, .vc = std::vector<char>{}, .s = "oraoraoraora" };
//     ASSERT_EQ(simple_object, check_object);
// }
//
// TEST(fromPb, struct) {
//     const auto json = nlohmann::json::parse(
//         R"({"i":123,"simple":{"i":456,"vc":[97,98,99],"s":"oraoraoraora"},"mi":{"i_key":7,"j_key":13}})"
//     );
//     const auto object = from<TestStruct>(json);
//     const TestStruct check_object{
//         .i = 123,
//         .simple = TestSimpleStruct{ .i = 456, .vc = std::vector{ 'a', 'b', 'c' }, .s = "oraoraoraora" },
//         .mi =
//             std::map<std::string, int>{
//                 { "i_key", 7 },
//                 { "j_key", 13 },
//             },
//     };
//     ASSERT_EQ(object, check_object);
// }
//
// TEST(fromPb, structSkipFields) {
//     const auto json = nlohmann::json::parse(R"({"i":123,"simple":{"i":456,"vc":[97,98,99]}})");
//     const auto object = from<TestStruct>(json);
//     const TestStruct check_object{
//         .i = 123,
//         .simple = TestSimpleStruct{ .i = 456, .vc = std::vector{ 'a', 'b', 'c' }, .s = tl::nullopt },
//         .mi = tl::nullopt,
//     };
//     ASSERT_EQ(object, check_object);
// }
//
// TEST(fromPb, structEmptyMap) {
//     const auto json =
//         nlohmann::json::parse(R"({"i":123,"simple":{"i":456,"vc":[97,98,99],"s":"oraoraoraora"},"mi":{}})");
//     const auto object = from<TestStruct>(json);
//     const TestStruct check_object{
//         .i = 123,
//         .simple = TestSimpleStruct{ .i = 456, .vc = std::vector{ 'a', 'b', 'c' }, .s = "oraoraoraora" },
//         .mi = std::map<std::string, int>{},
//     };
//     ASSERT_EQ(object, check_object);
// }

} // namespace sl::srlz
