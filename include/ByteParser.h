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

// ByteParser class for parsing byte buffers
class ByteParser {
public:
    enum class Side : std::uint8_t {
        None = 0,
        Bid = 1,
        Ask = 2,
    };

    enum class MessageType : std::uint8_t {
        None = 0,
        QuoteUpdate = 1,
        TradeMessage = 2,
    };

    enum class ParseError {
        TooShortBuffer,
        InvalidLength,
        InvalidPayloadLength,
        UnsupportedMessageType,
        InvalidQuoteSide,
        ParserError,
    };

    struct QuoteUpdateMessage {
        std::uint32_t instrumentID = 0;
        std::uint64_t price = 0;
        std::uint32_t quantity = 0;
        Side side = Side::None;
    };

    struct TradeMessage {
        std::uint32_t instrumentID = 0;
        std::uint64_t price = 0;
        std::uint32_t quantity = 0;
    };

    using PayloadMessage = std::variant<QuoteUpdateMessage, TradeMessage>;

    struct ParseData {
        std::uint16_t messageLength = 0;
        MessageType messageType = MessageType::None;
        std::uint32_t sequenceNumber = 0;
        PayloadMessage payload;
    };

    using Result = std::variant<ParseData, ParseError>;

    static Result parse(const std::span<const std::byte>) noexcept;
};
#endif //ALL_APP_BYTEPARSER_H
