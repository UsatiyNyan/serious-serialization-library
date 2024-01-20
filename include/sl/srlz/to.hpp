//
// Created by usatiynyan on 14.01.24.
//

#pragma once

#include "sl/srlz/field.hpp"

#include <range/v3/view/enumerate.hpp>

#include <sl/meta/func/const.hpp>
#include <sl/meta/traits/is_mapping.hpp>
#include <sl/meta/traits/is_range.hpp>
#include <sl/meta/tuple/for_each.hpp>
#include <sl/meta/tuple/tie_as_tuple.hpp>
#include <type_traits>

namespace sl::srlz {
namespace impl {

template <typename DstT>
struct init_to {
    // when T is mapping/range/tuple might make sense to preallocate data before assign_to
    template <typename SrcT, typename... Args>
    constexpr DstT operator()([[maybe_unused]] const SrcT& src, Args&&... args) const {
        return DstT(std::forward<Args>(args)...);
    }
};

template <typename DstT>
struct finalize_to {
    template <typename SrcT>
    constexpr void operator()([[maybe_unused]] DstT& dst, [[maybe_unused]] const SrcT& src) const {}
};

template <typename DstT>
struct assign_to {
    // mapping assignment
    constexpr void operator()(DstT& dst, std::string_view key, DstT&& dst_value) const {
        dst[key] = std::move(dst_value);
    }

    // range assignment
    constexpr void operator()(DstT& dst, [[maybe_unused]] std::size_t i, DstT&& dst_value) const {
        dst.push_back(std::move(dst_value));
    }
};

template <typename DstT, typename SrcT>
struct to {
    template <typename... Args>
    constexpr DstT operator()(const SrcT& src, Args&&... args) const {
        constexpr impl::init_to<DstT> init_to_f;
        DstT dst = init_to_f(src, std::forward<Args>(args)...);

        if constexpr (std::is_constructible_v<DstT, SrcT>) {
            dst = DstT(src);
        } else if constexpr (std::is_aggregate_v<SrcT>) {
            meta::for_each(
                [&](const auto& field) { reduce_to(dst, field, std::forward<Args>(args)...); }, meta::tie_as_tuple(src)
            );
        } else if constexpr (meta::is_mapping_v<SrcT>) {
            constexpr impl::assign_to<DstT> assign_to_f;
            constexpr impl::to<DstT, typename SrcT::mapped_type> to_f;
            for (const auto& [key, elem] : src) {
                assign_to_f(dst, std::string_view{ key }, to_f(elem, std::forward<Args>(args)...));
            }
        } else if constexpr (meta::is_range_v<SrcT>) {
            constexpr impl::assign_to<DstT> assign_to_f;
            constexpr impl::to<DstT, typename SrcT::value_type> to_f;
            for (const auto& [i, elem] : ranges::views::enumerate(src)) {
                assign_to_f(dst, i, to_f(elem, std::forward<Args>(args)...));
            }
        } else {
            static_assert(meta::const_t<std::false_type, SrcT>::value, "not implemented");
        }

        constexpr impl::finalize_to<DstT> finalize_to_f;
        finalize_to_f(dst, src);
        return dst;
    }

private:
    template <typename field_type, meta::string field_name, typename... Args>
    static constexpr void reduce_to(DstT& dst, const srlz::field<field_type, field_name>& field, Args&&... args) {
        constexpr impl::to<DstT, srlz::field<field_type, field_name>> to_f;
        to_f(field, std::forward<Args>(args)...) //
            .map([&dst](DstT&& dst_value) {
                constexpr impl::assign_to<DstT> assign_to_f;
                assign_to_f(dst, field_name.string_view(), std::move(dst_value));
            });
    }
};

template <typename DstT, typename field_type, meta::string field_name>
struct to<DstT, srlz::field<field_type, field_name>> {
    template <typename... Args>
    constexpr tl::optional<DstT> operator()(const srlz::field<field_type, field_name>& field, Args&&... args) const {
        return field.map([&](const field_type& field_value) {
            constexpr impl::to<DstT, field_type> to_f;
            return to_f(field_value, std::forward<Args>(args)...);
        });
    }
};

} // namespace impl

template <typename DstT, typename SrcT, typename... Args>
DstT to(const SrcT& src, Args&&... args) {
    constexpr impl::to<DstT, SrcT> to_f;
    return to_f(src, std::forward<Args>(args)...);
}

} // namespace sl::srlz
