#include "sl/srlz/field.hpp"

#include <map>
#include <string>
#include <vector>

namespace sl::srlz {

struct TestSimpleStruct {
    field<int, "i"> i;
    field<std::vector<char>, "vc"> vc;
    field<std::string, "s"> s;

    bool operator==(const TestSimpleStruct&) const = default;
};

struct TestStruct {
    field<int, "i"> i;
    field<TestSimpleStruct, "simple"> simple;
    field<std::map<std::string, int>, "mi"> mi;

    bool operator==(const TestStruct&) const = default;
};

} // namespace sl::srlz
