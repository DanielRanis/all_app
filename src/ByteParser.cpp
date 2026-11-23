//
// Created by daniel on 11/20/26.
//
#include "ByteParser.h"
#include <optional>
#include <utility>
#include <concepts>


//Anonymous namespace for Helper Functions
namespace {
    template<std::unsigned_integral T>
    std::optional<T> readBigEndian(const std::span<const std::byte> data) noexcept {
        if (data.empty()) {
            return std::nullopt;
        }

        T value = 0;
        for (const std::byte byte: data) {
            value = static_cast<T>((value << 8) | std::to_integer<unsigned int>(byte));
        }
        return value;
    }
}

ByteParser::Result ByteParser::parse(const std::span<const std::byte> data) noexcept {
    static constexpr std::size_t msgLengthBytes = 2;
    static constexpr std::size_t msgTypeBytes = 1;
    static constexpr std::size_t seqNumBytes = 4;
    static constexpr std::size_t headerSize = msgLengthBytes + msgTypeBytes + seqNumBytes;
    // QuoteUpdateMessage: 4 + 8 + 4 + 1 = 17 bytes
    // TradeMessage: 4 + 8 + 4 = 16 bytes
    static constexpr std::size_t payloadQuoteUpdateBytes = 17;
    static constexpr std::size_t payloadTradeMessageBytes = 16;
    static constexpr std::size_t instrumentIDBytes = 4;
    static constexpr std::size_t priceBytes = 8;
    static constexpr std::size_t quantityBytes = 4;
    static constexpr std::size_t sideBytes = 1;
    // Check data
    if (data.size() < headerSize) {
        return ParseError::TooShortBuffer;
    }

    // Parse and check message length
    const auto msgLength = readBigEndian<std::uint16_t>(data.subspan(0, msgLengthBytes));
    if (!msgLength || msgLength.value() > data.size() || msgLength.value() < headerSize) {
        return ParseError::InvalidLength;
    }

    const std::size_t payloadSize = msgLength.value() - headerSize;

    // Parse and check message type
    const auto msgType = readBigEndian<std::uint8_t>(data.subspan(msgLengthBytes, msgTypeBytes));
    if (!msgType || (static_cast<MessageType>(msgType.value()) != MessageType::QuoteUpdate && static_cast<MessageType>(
                         msgType.value()) != MessageType::TradeMessage)) {
        return ParseError::UnsupportedMessageType;
    }
    // Parse sequence number
    const auto seqNum = readBigEndian<std::uint32_t>(data.subspan(headerSize - seqNumBytes, seqNumBytes));

    // Parse Payload
    PayloadMessage payload;
    switch (static_cast<MessageType>(msgType.value())) {
        case MessageType::QuoteUpdate: {
            if (payloadSize != payloadQuoteUpdateBytes) {
                return ParseError::InvalidPayloadLength;
            }
            const auto quoteInstrumentID = readBigEndian<std::uint32_t>(data.subspan(headerSize, instrumentIDBytes));
            const auto quotePrice = readBigEndian<std::uint64_t>(
                data.subspan(headerSize + instrumentIDBytes, priceBytes));
            const auto quoteQuantity = readBigEndian<std::uint32_t>(
                data.subspan(headerSize + instrumentIDBytes + priceBytes, quantityBytes));
            const auto quoteSide = readBigEndian<std::uint8_t>(
                data.subspan(headerSize + instrumentIDBytes + priceBytes + quantityBytes, sideBytes));

            // Check side value
            if (static_cast<Side>(quoteSide.value()) != Side::Bid && static_cast<Side>(
                    quoteSide.
                    value()) != Side::Ask) {
                return ParseError::InvalidQuoteSide;
            }

            // Create QuoteUpdateMessage payload
            payload = QuoteUpdateMessage{
                .instrumentID = quoteInstrumentID.value(),
                .price = quotePrice.value(),
                .quantity = quoteQuantity.value(),
                .side = static_cast<Side>(quoteSide.value())
            };
            break;
        }
        case MessageType::TradeMessage: {
            if (payloadSize != payloadTradeMessageBytes) {
                return ParseError::InvalidPayloadLength;
            }
            const auto tradeInstrumentID = readBigEndian<std::uint32_t>(data.subspan(headerSize, instrumentIDBytes));
            const auto tradePrice = readBigEndian<std::uint64_t>(
                data.subspan(headerSize + instrumentIDBytes, priceBytes));
            const auto tradeQuantity = readBigEndian<std::uint32_t>(
                data.subspan(headerSize + instrumentIDBytes + priceBytes, quantityBytes));
            // Create TradeMessage payload
            payload = TradeMessage{
                .instrumentID = tradeInstrumentID.value(),
                .price = tradePrice.value(),
                .quantity = tradeQuantity.value()
            };
            break;
        }
        default:
            return ParseError::UnsupportedMessageType;
    }

    // Return Results
    auto parseData = ParseData{
        .messageLength = msgLength.value(), .messageType = static_cast<MessageType>(msgType.value()),
        .sequenceNumber = seqNum.value(),
        .payload = std::move(payload)
    };
    return parseData;
}
