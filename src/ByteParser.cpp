//
// Created by daniel on 11/20/26.
//
#include "ByteParser.h"

ByteParser::Result ByteParser::parse(std::span<const std::byte> data) noexcept {
    static constexpr std::size_t msgLengthNumBytes = 2;
    static constexpr std::size_t msgTypeLengthNumBytes = 1;
    static constexpr std::size_t seqNumLengthNumBytes = 4;
    static constexpr std::size_t headerSize = msgLengthNumBytes + msgTypeLengthNumBytes + seqNumLengthNumBytes;

    // Check data
    if (data.size() < headerSize) {
        return ParseError::TooShortBuffer;
    }

    // Single Byte Conversion
    const auto byte = [&](const std::size_t index) { return std::to_integer<std::uint8_t>(data[index]); };

    // Check message length
    const auto msgLength = static_cast<std::uint16_t>((static_cast<std::uint16_t>(byte(0)) << 8) | byte(
                                                          msgLengthNumBytes - 1));
    if (msgLength > data.size() || msgLength < headerSize) {
        return ParseError::InvalidLength;
    }

    // Check message type
    const auto msgType = byte(msgLengthNumBytes + msgTypeLengthNumBytes - 1);
    if (msgType != 1 && msgType != 2) {
        return ParseError::UnsupportedMessageType;
    }

    const auto seqNum = (static_cast<std::uint32_t>(byte(headerSize - 4)) << 24)
                        | (static_cast<std::uint32_t>(byte(headerSize - 3)) << 16)
                        | (static_cast<std::uint32_t>(byte(headerSize - 2)) << 8)
                        | static_cast<std::uint32_t>(byte(headerSize - 1));


    auto parseData = ParseData{
        .messageLength = msgLength, .messageType = msgType, .sequenceNumber = seqNum,
        .payload = data.subspan(headerSize).first(msgLength - headerSize)
    };
    return parseData;
}
