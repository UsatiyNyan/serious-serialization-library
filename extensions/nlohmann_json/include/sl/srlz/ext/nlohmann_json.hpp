#include "sl/srlz/to.hpp"

#include <nlohmann/json.hpp>
#include <type_traits>

namespace sl::srlz {

template <>
struct impl::init_to<nlohmann::json> {
    template <typename SrcT, typename... Args>
    nlohmann::json operator()(const SrcT&, Args&&...) const {
        if constexpr (meta::is_mapping_v<SrcT>) {
            return nlohmann::json::object();
        } else if constexpr (meta::is_range_v<SrcT>) {
            return nlohmann::json::array();
        } else {
            return nlohmann::json{};
        }
    }
};

} // namespace sl::srlz
