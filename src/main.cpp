#include <iostream>
#include <array>
#include "ByteParser.h"

int main() {
    using ParseData = ByteParser::ParseData;
    using ParseError = ByteParser::ParseError;
    // Example buffers
    constexpr std::byte quoteBuffer[] = {
        // Header: 7 bytes
        std::byte{0}, std::byte{24}, std::byte{1}, std::byte{0}, std::byte{0}, std::byte{0}, std::byte{1},
        // Payload: 17 bytes
        std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF},
        std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF},
        std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF},
        std::byte{0xFF}, std::byte{1}
    };
    constexpr std::byte tradeBuffer[] = {
        // Header: 7 bytes
        std::byte{0}, std::byte{23}, std::byte{2}, std::byte{0}, std::byte{0}, std::byte{0}, std::byte{1},
        // Payload: 16 bytes
        std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF},
        std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF},
        std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF},
        std::byte{0xFF}
    };
    const std::array<std::span<const std::byte>, 2> testBuffers{
        std::span<const std::byte>{quoteBuffer},
        std::span<const std::byte>{tradeBuffer},
    };

    for (const auto buffer: testBuffers) {
        if (const auto result = ByteParser::parse(buffer); std::holds_alternative<
            ParseError>(result)) {
            if (const auto error = std::get<ParseError>(result); error == ParseError::InvalidLength) {
                std::cout << "Error: Invalid length" << std::endl;
            } else if (error == ParseError::UnsupportedMessageType) {
                std::cout << "Error: Unsupported message type" << std::endl;
            } else if (error == ParseError::TooShortBuffer) {
                std::cout << "Error: Too short buffer" << std::endl;
            } else if (error == ParseError::InvalidPayloadLength) {
                std::cout << "Error: Invalid payload length" << std::endl;
            } else if (error == ParseError::InvalidQuoteSide) {
                std::cout << "Error: Invalid quote side" << std::endl;
            } else {
                std::cout << "Error: Unknown error" << std::endl;
            }
        } else {
            const auto &[messageLength, messageType, sequenceNumber, payload] = std::get<ParseData>(result);
            std::cout << "Message Length: " << messageLength << std::endl;
            std::cout << "Message Type: " << static_cast<int>(messageType) << std::endl;
            std::cout << "Sequence Number: " << sequenceNumber << std::endl;
        }
    }

    return 0;
}
