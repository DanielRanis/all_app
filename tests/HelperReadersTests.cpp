//
// Created by daniel on 11/23/26.
//

#include "HelperReaders.h"

#include <array>
#include <cstdint>
#include <gtest/gtest.h>

TEST(HelperReadersTests, ReadsBigEndian16BitValue) {
    constexpr std::array bytes{std::byte{0x12}, std::byte{0x34}};

    const auto result = HelperReaders::readBigEndian<std::uint16_t>(bytes);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0x1234);
}

TEST(HelperReadersTests, ReadsBigEndian32BitValue) {
    constexpr std::array bytes{
        std::byte{0x12}, std::byte{0x34}, std::byte{0x56}, std::byte{0x78}
    };

    const auto result = HelperReaders::readBigEndian<std::uint32_t>(bytes);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0x12345678);
}

TEST(HelperReadersTests, ReadsBigEndian64BitValue) {
    constexpr std::array bytes{
        std::byte{0x01}, std::byte{0x23}, std::byte{0x45}, std::byte{0x67},
        std::byte{0x89}, std::byte{0xab}, std::byte{0xcd}, std::byte{0xef}
    };

    const auto result = HelperReaders::readBigEndian<std::uint64_t>(bytes);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0x0123456789abcdefULL);
}

TEST(HelperReadersTests, RejectsEmptyOrIncorrectlySizedInput) {
    constexpr std::array<std::byte, 0> empty{};
    constexpr std::array tooShort{std::byte{0x12}};
    constexpr std::array tooLong{std::byte{0x12}, std::byte{0x34}, std::byte{0x56}};

    EXPECT_FALSE(HelperReaders::readBigEndian<std::uint16_t>(empty).has_value());
    EXPECT_FALSE(HelperReaders::readBigEndian<std::uint16_t>(tooShort).has_value());
    EXPECT_FALSE(HelperReaders::readBigEndian<std::uint16_t>(tooLong).has_value());
}

TEST(HelperReadersTests, ReadsFromOffsetOne) {
    constexpr std::array bytes{
        std::byte{0xFF}, // padding byte
        std::byte{0x12},
        std::byte{0x34}
    };

    const auto input = std::span<const std::byte>{bytes}.subspan(1, 2);
    const auto result = HelperReaders::readBigEndian<std::uint16_t>(input);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0x1234);
}
