#include "sl/srlz/to.hpp"

#include <nlohmann/json.hpp>
#include <type_traits>

namespace sl::srlz {

template <typename CharT>
struct impl::to<nlohmann::json, std::basic_string<CharT>> {
    template <typename... Args>
    nlohmann::json operator()(const std::basic_string<CharT>& value, Args&&...) const {
        return value;
    }
};

template <typename CharT>
struct impl::to<nlohmann::json, std::basic_string_view<CharT>> {
    template <typename... Args>
    nlohmann::json operator()(const std::basic_string_view<CharT>& value, Args&&...) const {
        return value;
    }
};

template <>
struct impl::init_to<nlohmann::json> {
    template <typename T, typename... Args>
    nlohmann::json operator()(const T&, Args&&...) const {
        return nlohmann::json{};
    }

    template <typename T, typename... Args>
        requires(meta::is_range_v<T> && !meta::is_mapping_v<T>)
    nlohmann::json operator()(const T&, Args&&...) const {
        return nlohmann::json::array();
    }

    template <typename T, typename... Args>
        requires meta::is_mapping_v<T>
    nlohmann::json operator()(const T&, Args&&...) const {
        return nlohmann::json::object();
    }
};

} // namespace sl::srlz
