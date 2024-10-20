#ifndef UTILS_BITUTILS_H
#define UTILS_BITUTILS_H

#include <cstring>
#include <type_traits>

namespace utils {

template <class To, class From>
inline To bit_cast(const From &src)
{
    static_assert(sizeof(To) == sizeof(From), "size of the types must be equal");
    static_assert(std::is_trivially_copyable_v<To> && std::is_trivially_copyable_v<From>,
                  "source and destination types must be trivially copyable");
    static_assert(std::is_trivially_constructible_v<To>, "destination type must be default constructible");

    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}

}  // namespace utils

#endif  // UTILS_BITUTILS_H
