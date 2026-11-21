//
// Created by daniel on 11/20/26.
//

/*
 * Byte Buffer Parser:
 * Message Format (Big Endian):
 * |Message length (2 Bytes)|Message type (1 Byte)|Sequence number (4 Bytes)|Payload (variable length)|
 */

#ifndef ALL_APP_BYTEPARSER_H
#define ALL_APP_BYTEPARSER_H
#include <cstdint>
#include <cstddef>
#include <span>
#include <variant>


class ByteParser {
public:
    enum class ParseError {
        TooShortBuffer,
        InvalidLength,
        UnsupportedMessageType,
    };

    struct ParseData {
        std::uint16_t messageLength = 0;
        std::uint8_t messageType = 0;
        std::uint32_t sequenceNumber = 0;
        std::span<const std::byte> payload{};
    };

    using Result = std::variant<ParseData, ParseError>;

    static Result parse(std::span<const std::byte>) noexcept;
};
#endif //ALL_APP_BYTEPARSER_H
