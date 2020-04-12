#pragma once

#include <array>
#include <cstdint>
#include <type_traits>

namespace {
template <typename T, std::size_t rank, std::size_t... sizes>
struct DimensionalArrayExplicitRank;

template <std::size_t rank, std::size_t... sizes>
constexpr std::size_t GetRankSize() {
    constexpr std::size_t size_array[] = {sizes...};
    return size_array[rank - 1];
}

template <typename T, std::size_t rank, std::size_t... sizes>
using GetArrayImplType = std::array<
    std::conditional_t<rank == 1, T, DimensionalArrayExplicitRank<T, rank - 1, sizes...>>,
    GetRankSize<rank, sizes...>()>;

template <typename T, std::size_t rank_param, std::size_t... sizes>
struct DimensionalArrayExplicitRank : public GetArrayImplType<T, rank_param, sizes...> {
    static constexpr std::size_t rank = rank_param;
    static_assert(rank > 0, "Attempted to create dimensional array with rank less than 1");
    using ArrayImplType = GetArrayImplType<T, rank, sizes...>;
    using ArrayImplType::ArrayImplType;
};
} // namespace

template <typename T, std::size_t... sizes>
using DimensionalArray = DimensionalArrayExplicitRank<T, sizeof...(sizes), sizes...>;

template <typename T, std::size_t size>
using SquareArray = DimensionalArray<T, size, size>;

template <typename T, std::size_t size>
using CubeArray = DimensionalArray<T, size, size, size>;
