//
// Created by usatiynyan on 19.01.24.
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

template <typename SrcT>
struct init_from {
    template <typename DstT, typename... Args>
    constexpr DstT operator()([[maybe_unused]] const SrcT& src, Args&&... args) const {
        return DstT(std::forward<Args>(args)...);
    }
};

template <typename SrcT>
struct finalize_from {
    template <typename DstT>
    constexpr void operator()([[maybe_unused]] DstT& dst, [[maybe_unused]] const SrcT& src) const {}
};

template <typename SrcT>
struct find_from {
    constexpr tl::optional<const SrcT&> operator()(const SrcT& src, std::string_view key) const {
        if (const auto it = src.find(key); it != src.end()) {
            return *it;
        }
        return tl::nullopt;
    }
};

template <typename SrcT>
struct items_from {
    constexpr auto operator()(const SrcT& src) const { return src.items(); }
};

template <typename DstT>
struct assign_from {
    // mapping assignment
    constexpr void operator()(DstT& dst, std::string_view key, DstT::mapped_type&& dst_value) const {
        dst.insert_or_assign(key, std::move(dst_value));
    }

    // range assignment
    constexpr void operator()(DstT& dst, std::size_t i, DstT::value_type&& dst_value) const {
        constexpr bool is_array_like = std::is_same_v<decltype(DstT::size()), std::size_t> || std::is_array_v<DstT>;
        using dst_vt = typename DstT::value_type;
        constexpr bool is_vector_like =
            std::is_same_v<decltype(std::declval<DstT>().push_back(std::declval<dst_vt>())), void>;
        using dst_cit = typename DstT::const_iterator;
        using dst_it = typename DstT::iterator;
        constexpr bool is_list_like = //
            std::is_same_v<
                decltype(std::declval<DstT>().insert(std::declval<dst_cit>(), std::declval<dst_vt>())),
                dst_it>;

        if constexpr (is_array_like) {
            dst[i] = std::move(dst_value);
        } else if constexpr (is_vector_like) {
            dst.push_back(std::move(dst_value));
        } else if constexpr (is_list_like) {
            dst.insert(dst.end(), std::move(dst_value));
        } else {
            static_assert(meta::const_t<std::false_type, DstT>::value, "not implemented");
        }
    }
};

template <typename DstT, typename SrcT>
struct from {
    template <typename... Args>
    constexpr DstT operator()(const SrcT& src, Args&&... args) const {
        constexpr impl::init_from<SrcT> init_from_f;
        DstT dst = init_from_f.template operator()<DstT>(src, std::forward<Args>(args)...);

        if constexpr (std::is_constructible_v<DstT, SrcT>) {
            dst = DstT(src);
        } else if constexpr (std::is_aggregate_v<DstT>) {
            meta::for_each(
                [&](auto& field) { reduce_from(field, src, std::forward<Args>(args)...); }, meta::tie_as_tuple(dst)
            );
        } else if constexpr (meta::is_mapping_v<DstT>) {
            constexpr impl::items_from<SrcT> items_from_f;
            constexpr impl::assign_from<DstT> assign_from_f;
            constexpr impl::from<typename DstT::mapped_type, SrcT> from_f;
            for (const auto& [key, elem] : items_from_f(src)) {
                assign_from_f(dst, std::string_view{ key }, from_f(elem, std::forward<Args>(args)...));
            }
        } else if constexpr (meta::is_range_v<DstT>) {
            constexpr impl::assign_from<DstT> assign_from_f;
            constexpr impl::from<typename DstT::value_type, SrcT> from_f;
            for (const auto& [i, elem] : ranges::views::enumerate(src)) {
                assign_from_f(dst, i, from_f(elem, std::forward<Args>(args)...));
            }
        } else {
            static_assert(meta::const_t<std::false_type, DstT>::value, "not implemented");
        }

        constexpr impl::finalize_from<SrcT> finalize_from_f;
        finalize_from_f(dst, src);
        return dst;
    }

private:
    template <typename field_type, meta::string field_name, typename... Args>
    static constexpr void reduce_from(srlz::field<field_type, field_name>& field, const SrcT& src, Args&&... args) {
        constexpr impl::from<srlz::field<field_type, field_name>, SrcT> from_f;
        field = from_f(src, std::forward<Args>(args)...);
    }
};

template <typename field_type, meta::string field_name, typename SrcT>
struct from<srlz::field<field_type, field_name>, SrcT> {
    template <typename... Args>
    constexpr tl::optional<field_type> operator()(const SrcT& src, Args&&... args) const {
        constexpr impl::find_from<SrcT> find_from_f;
        return find_from_f(src, field_name.string_view()) //
            .map([&](const SrcT& src_value) {
                constexpr impl::from<field_type, SrcT> from_f;
                return from_f(src_value, std::forward<Args>(args)...);
            });
    }
};

} // namespace impl

template <typename DstT, typename SrcT, typename... Args>
DstT from(const SrcT& src, Args&&... args) {
    constexpr impl::from<DstT, SrcT> from_f;
    return from_f(src, std::forward<Args>(args)...);
}

} // namespace sl::srlz
