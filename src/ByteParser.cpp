//
// Created by daniel on 11/20/26.
//
#include "ByteParser.h"
#include "HelperReaders.h"
#include <optional>
#include <utility>


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
    const auto msgLength = HelperReaders::readBigEndian<std::uint16_t>(
        data.subspan(0, msgLengthBytes));
    if (!msgLength || msgLength.value() > data.size() || msgLength.value() < headerSize) {
        return ParseError::InvalidLength;
    }

    const std::size_t payloadSize = msgLength.value() - headerSize;

    // Parse and check message type
    const auto msgType = HelperReaders::readBigEndian<std::uint8_t>(
        data.subspan(msgLengthBytes, msgTypeBytes));
    if (!msgType || (static_cast<MessageType>(msgType.value()) != MessageType::QuoteUpdate && static_cast<MessageType>(
                         msgType.value()) != MessageType::TradeMessage)) {
        return ParseError::UnsupportedMessageType;
    }
    // Parse sequence number
    const auto seqNum = HelperReaders::readBigEndian<std::uint32_t>(data.subspan(
        headerSize - seqNumBytes, seqNumBytes));

    // Parse Payload
    PayloadMessage payload;
    switch (static_cast<MessageType>(msgType.value())) {
        case MessageType::QuoteUpdate: {
            if (payloadSize != payloadQuoteUpdateBytes) {
                return ParseError::InvalidPayloadLength;
            }
            const auto quoteInstrumentID = HelperReaders::readBigEndian<std::uint32_t>(
                data.subspan(headerSize, instrumentIDBytes));
            const auto quotePrice = HelperReaders::readBigEndian<std::uint64_t>(
                data.subspan(headerSize + instrumentIDBytes, priceBytes));
            const auto quoteQuantity = HelperReaders::readBigEndian<std::uint32_t>(
                data.subspan(headerSize + instrumentIDBytes + priceBytes, quantityBytes));
            const auto quoteSide = HelperReaders::readBigEndian<std::uint8_t>(
                data.subspan(headerSize + instrumentIDBytes + priceBytes + quantityBytes, sideBytes));
            // Check for valid optional values
            if (!quoteInstrumentID.has_value() || !quotePrice.has_value() || !quoteQuantity.has_value() || !quoteSide.
                has_value()) {
                return ParseError::ParserError;
            }
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
            const auto tradeInstrumentID = HelperReaders::readBigEndian<std::uint32_t>(
                data.subspan(headerSize, instrumentIDBytes));
            const auto tradePrice = HelperReaders::readBigEndian<std::uint64_t>(
                data.subspan(headerSize + instrumentIDBytes, priceBytes));
            const auto tradeQuantity = HelperReaders::readBigEndian<std::uint32_t>(
                data.subspan(headerSize + instrumentIDBytes + priceBytes, quantityBytes));
            // Check for valid optional values
            if (!tradeInstrumentID.has_value() || !tradePrice.has_value() || !tradeQuantity.has_value()) {
                return ParseError::ParserError;
            }
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
