#pragma once

#include <array>
#include <cmath>
#include <execution>
#include <numeric>
#include <type_traits>

namespace {
constexpr auto parallel = std::execution::par_unseq;
}


template <typename T, std::size_t size_param>
struct VecBase : public std::array<T, size_param> {
    using ArrayType = std::array<T, size_param>;
    using ArrayType::operator[];
    using ArrayType::begin;
    using ArrayType::end;

    constexpr VecBase() = default;

    template <typename... U, typename = std::enable_if_t<sizeof...(U) == size_param>>
    constexpr VecBase(U... elements) : ArrayType{elements...} {}

    constexpr VecBase(T value) {
        std::fill(parallel, begin(), end(), value);
    }

    template <typename U>
    constexpr explicit operator VecBase<U, size_param>() {
        VecBase<U, size_param> casted;
        std::transform(parallel, begin(), end(), casted.begin(),
                       [](auto x) { return static_cast<U>(x); });
        return casted;
    }

    template <typename U>
    constexpr auto as() {
        return VecBase<U, size_param>(*this);
    }

    template <typename U>
    constexpr VecBase<decltype(T{} - U{}), size_param> operator-(
        VecBase<U, size_param> subtrahend) const {
        VecBase<decltype(T{} - U{}), size_param> difference;
        for (std::size_t idx{0}; idx < size_param; ++idx) {
            difference[idx] = (*this)[idx] - subtrahend[idx];
        }
        return difference;
    }

    template <typename U>
    constexpr auto operator*(VecBase<U, size_param> multiplicand) const {
        VecBase<decltype(T{} * U{}), size_param> product;
        for (std::size_t idx{0}; idx < size_param; ++idx) {
            product[idx] = (*this)[idx] * multiplicand[idx];
        }
        return product;
    }

    constexpr auto sum() const {
        return std::accumulate(begin(), end(), T{});
    }

    template <typename U>
    constexpr auto dot(VecBase<U, size_param> rhs) const {
        return (*this * rhs).sum();
    }

    constexpr auto length() const {
        return std::sqrt(this->dot(*this));
    }

    template <typename U>
    constexpr auto distance(VecBase<U, size_param> rhs) const {
        return (*this - rhs).length();
    }

    template <typename = std::enable_if_t<size_param >= 1>>
    constexpr auto r() const {
        return (*this)[0];
    }

    template <typename = std::enable_if_t<size_param >= 1>>
    constexpr auto& r() {
        return (*this)[0];
    }

    template <typename = std::enable_if_t<size_param >= 2>>
    constexpr auto g() const {
        return (*this)[1];
    }

    template <typename = std::enable_if_t<size_param >= 2>>
    constexpr auto& g() {
        return (*this)[1];
    }

    template <typename = std::enable_if_t<size_param >= 3>>
    constexpr auto b() const {
        return (*this)[2];
    }

    template <typename = std::enable_if_t<size_param >= 3>>
    constexpr auto& b() {
        return (*this)[2];
    }
};
