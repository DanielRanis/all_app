//
// Created by daniel on 11/23/26.
//

#ifndef ALL_APP_HELPERREADERS_H
#define ALL_APP_HELPERREADERS_H
#include <concepts>
#include <cstddef>
#include <optional>
#include <span>

// Namespace for Helper Reader Functions
namespace HelperReaders {
    template<std::unsigned_integral T>
    std::optional<T> readBigEndian(const std::span<const std::byte> data) noexcept {
        if (data.empty() || data.size() != sizeof(T)) {
            return std::nullopt;
        }

        T value = 0;
        for (const std::byte byte: data) {
            value = static_cast<T>((value << 8) | std::to_integer<unsigned int>(byte));
        }
        return value;
    }
}


#endif //ALL_APP_HELPERREADERS_H
