#include <concepts>

template <std::unsigned_integral U>
auto CeilDiv(U dividend, U divisor) {
    return (dividend + divisor - 1) / divisor;
}