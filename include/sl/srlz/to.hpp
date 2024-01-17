//
// Created by usatiynyan on 14.01.24.
//

#pragma once

#include "sl/srlz/field.hpp"

#include <concepts>
#include <range/v3/view/enumerate.hpp>

#include <sl/meta/traits.hpp>
#include <sl/meta/tuple/decay.hpp>
#include <sl/meta/tuple/for_each.hpp>
#include <sl/meta/tuple/tie_as_tuple.hpp>
#include <type_traits>

namespace sl::srlz {
namespace impl {

template <typename DstT, typename T>
struct to;

template <typename DstT>
struct init_to {
    // when T is mapping/range/tuple might make sense to preallocate data before assign_to
    template <typename T, typename... Args>
    constexpr DstT operator()([[maybe_unused]] const T& value, Args&&... args) const {
        return DstT{ std::forward<Args>(args)... };
    }
};

template <typename DstT>
struct finalize_to {
    template <typename T>
    constexpr void operator()([[maybe_unused]] DstT& dst, [[maybe_unused]] const T& value) const {}
};

template <typename DstT>
struct field_to {
    template <typename field_type, meta::string field_name>
    struct dst {
        using type = DstT;
    };
};

template <typename DstT>
struct assign_to {
    // field assignment
    template <typename field_dst_type, meta::string field_name>
    constexpr void operator()(DstT& dst, field_dst_type dst_value) const {
        dst[field_name.chars] = std::move(dst_value);
    }

    // mapping assignment
    template <typename field_dst_type>
    constexpr void operator()(DstT& dst, std::string_view field_name, field_dst_type dst_value) const {
        dst[field_name.data()] = std::move(dst_value);
    }

    // range assignment
    template <typename field_dst_type>
    constexpr void operator()(DstT& dst, std::size_t i, field_dst_type dst_value) const {
        dst[i] = std::move(dst_value);
    }
};

template <typename DstT, typename T>
    requires meta::is_specialization_v<T, std::tuple>
struct to<DstT, T> {
    template <typename... Args>
    constexpr DstT operator()(const T& value, Args&&... args) const {
        constexpr impl::init_to<DstT> init_to_f;
        DstT dst = init_to_f(value, std::forward<Args>(args)...);
        meta::for_each([&dst](const auto& field) { reduce_to(dst, field); }, value);
        constexpr impl::finalize_to<DstT> finalize_to_f;
        finalize_to_f(dst, value);
        return dst;
    }

private:
    template <typename field_type, meta::string field_name, typename... Args>
    static constexpr void reduce_to(DstT& dst, const srlz::field<field_type, field_name>& field, Args&&... args) {
        using field_to_f = impl::field_to<DstT>;
        using field_dst_type = typename field_to_f::template dst<field_type, field_name>::type;
        field.map([&](const field_type& field_value) {
            constexpr impl::assign_to<DstT> assign_to_f;
            constexpr impl::to<field_dst_type, field_type> to_f;
            assign_to_f.template operator()<field_dst_type, field_name> //
                (dst, to_f(field_value, std::forward<Args>(args)...));
        });
    }
};

template <typename DstT, typename T>
    requires std::is_aggregate_v<T>
struct to<DstT, T> {
    template <typename... Args>
    constexpr DstT operator()(const T& value, Args&&... args) const {
        const auto value_as_tuple = meta::tie_as_tuple(value);
        constexpr impl::to<DstT, std::decay_t<decltype(value_as_tuple)>> to_f;
        return to_f(value_as_tuple, std::forward<Args>(args)...);
    }
};

template <typename DstT, typename T>
    requires(meta::is_range_v<T> && !meta::is_mapping_v<T>)
struct to<DstT, T> {
    template <typename... Args>
    constexpr DstT operator()(const T& value, Args&&... args) const {
        constexpr impl::init_to<DstT> init_to_f;
        DstT dst = init_to_f(value, std::forward<Args>(args)...);
        constexpr impl::to<DstT, typename T::value_type> to_f;
        constexpr impl::assign_to<DstT> assign_to_f;
        for (const auto& [i, elem] : ranges::views::enumerate(value)) {
            assign_to_f(dst, i, to_f(elem, std::forward<Args>(args)...));
        }
        constexpr impl::finalize_to<DstT> finalize_to_f;
        finalize_to_f(dst, value);
        return dst;
    }
};

template <typename DstT, typename T>
    requires meta::is_mapping_v<T>
struct to<DstT, T> {
    template <typename... Args>
    constexpr DstT operator()(const T& value, Args&&... args) const {
        constexpr impl::init_to<DstT> init_to_f;
        DstT dst = init_to_f(value, std::forward<Args>(args)...);
        constexpr impl::to<DstT, typename T::mapped_type> to_f;
        constexpr impl::assign_to<DstT> assign_to_f;
        for (const auto& [key, elem] : value) {
            assign_to_f(dst, std::string_view{ key }, to_f(elem, std::forward<Args>(args)...));
        }
        constexpr impl::finalize_to<DstT> finalize_to_f;
        finalize_to_f(dst, value);
        return dst;
    }
};

} // namespace impl

template <typename DstT, typename T, typename... Args>
DstT to(const T& value, Args&&... args) {
    constexpr impl::to<DstT, T> to_f;
    return to_f(value, std::forward<Args>(args)...);
}

} // namespace sl::srlz
